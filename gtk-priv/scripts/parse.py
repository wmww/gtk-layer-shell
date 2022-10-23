'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import logging
import re

from ast import *

logger = logging.getLogger(__name__)

def remove_comments(code):
    return re.sub(r'(/\*.*?\*/|//.*?\n)', '', code, flags=re.DOTALL)

def parse_type(code):
    code = code.strip()
    if code.endswith('*'):
        return PtrType(parse_type(code[:-1]))
    elif code.startswith('const '):
        return ConstType(parse_type(code[5:]))
    if code.endswith(' const'):
        return ConstType(parse_type(code[:5]))
    elif is_std_type(code):
        return StdType(code)
    elif re.search(r'^((struct|enum)\s+)?\w+$', code):
        return CustomType(code)
    else:
        assert False, 'Unknown type `' + code + '`'

def parse_property(code):
    code = code.strip()
    bit_field = None
    bit_field_split = code.rsplit(':', 1)
    if len(bit_field_split) > 1:
        code = bit_field_split[0].strip()
        bit_field = int(bit_field_split[1].strip())
    fp = re.search(r'^(.*)\(\s*(\*\s*)+(\w*)\s*\)\s*\((.*)\)$', code)
    if fp:
        assert bit_field is None
        ret = parse_type(fp.group(1))
        pointer_count = fp.group(2).count('*') - 1
        assert pointer_count == 0, (
            'Function pointers with multiple levels of indirection not supported '
            '(indirection count: ' + str(pointer_count) + ' for ' + code + ')')
        name = fp.group(3)
        if name == '':
            name = None
        args = []
        for i in fp.group(4).split(','):
            args.append(parse_property(i))
        c_type = FpType(ret, args)
        return PropertyNode(c_type, name, None)
    array = re.search(r'^(.*)\[(.*)\]$', code)
    if array:
        prop = parse_property(array.group(1))
        prop.c_type = ArrayType(prop.c_type, array.group(2).strip())
        return prop
    normal = re.search(r'^((\w+\s*[^\w\,])+(\*\s*)*)([\w\s\,]*)$', code)
    if normal:
        c_type = parse_type(normal.group(1))
        names = normal.group(4).split(',')
        names = [name.strip() for name in names if name.strip()]
        if len(names) > 1:
            nodes = []
            for name in names:
                nodes.append(PropertyNode(c_type, name, None))
                nodes[-1].statement = True
            return ListNode(nodes)
        elif len(names) == 1:
            return PropertyNode(c_type, names[0], bit_field)
        else:
            return PropertyNode(c_type, None, bit_field)
    assert False, 'Could not parse property `' + code + '`'

def parse_token_list(tokens, i):
    nodes = []
    while i < len(tokens):
        token = tokens[i].strip()
        if token == '' or token == ';':
            pass
        elif token == 'struct' and tokens[i + 1].strip() == '{':
            i, node = parse_token_list(tokens, i + 2)
            i += 1
            assert i < len(tokens)
            name = tokens[i].strip()
            assert re.search(r'^\w+$', name)
            struct = SubStructNode(name, node)
            nodes.append(struct)
        elif token == '}':
            break
        else:
            node = parse_property(token)
            if isinstance(node, PropertyNode):
                node.statement = True
            nodes.append(node)
        i += 1
    return i, ListNode(nodes)

def parse_ast(code):
    code = remove_comments(code)
    code = code.replace('\n', ' ')
    code = code.replace('\t', ' ')
    tokens = re.split(r'([;\{\}])', code)
    i, node = parse_token_list(tokens, 0)
    assert i == len(tokens), code
    return node
