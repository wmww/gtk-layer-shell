'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import logging
import re

from version import Version
import code

logger = logging.getLogger(__name__)

def remove_comments(code):
    return re.sub(r'(/\*.*?\*/|//.*?\n)', '', code, flags=re.DOTALL)

# These can be combined in any order, we do not check that it is valid
std_types = set([
    'void',
    'int',
    'char',
    'signed',
    'unsigned',
    'short',
    'long',
    'float',
    'double',
    'uint32_t',
    'gint',
    'guint',
    'gchar',
    'gboolean',
    'gpointer',
])

def is_std_type(name):
    for i in name.split():
        if not i in std_types:
            return False
    return True

class CType:
    def __str__(self):
        return self.str_left(False) + self.str_right(False)

    def str_right(self, resolved):
        return ''

    def __eq__(self, other):
        return str(self) == str(other)

class StdType(CType):
    def __init__(self, name):
        assert isinstance(name, str)
        assert is_std_type(name), name
        self.name = name

    def resolve(self, ctx):
        pass

    def str_left(self, resolved):
        return self.name

class CustomType(CType):
    def __init__(self, name):
        assert isinstance(name, str)
        self.explicit_struct = False
        if name.startswith('struct'):
            name = name[6:].strip()
            self.explicit_struct = True
        self.explicit_enum = False
        if name.startswith('enum'):
            name = name[4:].strip()
            self.explicit_enum = True
        self.name = name
        self.version = None
        self.resolved_struct = None

    def resolve(self, ctx):
        if self.explicit_enum:
            struct = None
        elif self.explicit_struct:
            struct = ctx.project.lookup_struct_name(self.name)
        else:
            struct = ctx.project.lookup_typedef(self.name)

        if struct:
            self.resolved_struct = struct
            self.version = ctx.version

    def str_left(self, resolved):
        if self.resolved_struct and resolved:
            v = self.resolved_struct.lookup_version(self.version)
            return 'struct ' + v.versioned_struct_name()
        else:
            result = ''
            if self.explicit_struct:
                result += 'struct '
            if self.explicit_enum:
                result += 'enum '
            result += self.name
            return result

class PtrType(CType):
    def __init__(self, inner):
        assert isinstance(inner, CType)
        self.inner = inner

    def resolve(self, ctx):
        self.inner.resolve(ctx)

    def str_left(self, resolved):
        result = self.inner.str_left(False)
        if not isinstance(self.inner, PtrType):
            result += ' '
        result += '*'
        return result

    def str_right(self, resolved):
        return self.inner.str_right(False)

class ConstType(CType):
    def __init__(self, inner):
        assert isinstance(inner, CType)
        self.inner = inner

    def resolve(self, ctx):
        self.inner.resolve(ctx)

    def str_left(self, resolved):
        return 'const ' + self.inner.str_left(resolved)

    def str_right(self, resolved):
        return self.inner.str_right(resolved)

# Note the first (and usually only) layer of "pointer" is consumed by this type
# Do not wrap in a PtrType in most cases
class FpType(CType):
    def __init__(self, return_type, arg_list):
        assert isinstance(return_type, CType)
        assert isinstance(arg_list, list)
        for i in arg_list:
            assert isinstance(i, PropertyNode), str(i) + ' is not a property node'
        self.return_type = return_type
        self.arg_list = arg_list

    def resolve(self, ctx):
        self.return_type.resolve(ctx)
        for arg in self.arg_list:
            arg.resolve(ctx)

    def str_left(self, resolved):
        return str(self.return_type) + ' (*'

    def str_right(self, resolved):
        return ') (' + str(', '.join([str(t) for t in self.arg_list])) + ')'

class ArrayType(CType):
    def __init__(self, inner, size):
        assert isinstance(inner, CType)
        assert isinstance(size, str)
        self.inner = inner
        self.size = size

    def resolve(self, ctx):
        self.inner.resolve(ctx)

    def str_left(self, resolved):
        if resolved:
            return self.inner.str_left(True)
        else:
            return self.inner.str_left(False) + '*'

    def str_right(self, resolved):
        if resolved:
            return '[' + self.size + ']' + self.inner.str_right(True)
        else:
            return self.inner.str_right(False)

class AstNode:
    def __eq__(self, other):
        return str(self) == str(other)

class PropertyNode(AstNode):
    def __init__(self, c_type, name, bit_field):
        assert isinstance(c_type, CType)
        assert name is None or isinstance(name, str)
        assert bit_field is None or isinstance(bit_field, int)
        self.c_type = c_type
        self.name = name
        self.statement = False
        self.bit_field = bit_field

    def resolve(self, ctx):
        self.c_type.resolve(ctx)

    def get_property_list(self, prefix):
        return [(self.c_type, prefix + self.name)]

    def __str__(self):
        result = self.c_type.str_left(True)
        if self.name:
            if not isinstance(self.c_type, PtrType):
                result += ' '
            result += self.name
        result += self.c_type.str_right(True)
        if self.bit_field:
            result += ' : ' + str(self.bit_field)
        if self.statement:
            result += ';\n'
        return result

class ListNode(AstNode):
    def __init__(self, nodes):
        assert isinstance(nodes, list)
        for node in nodes:
            assert isinstance(node, AstNode)
        self.nodes = nodes

    def resolve(self, ctx):
        for node in self.nodes:
            node.resolve(ctx)

    def get_property_list(self, prefix):
        result = []
        for node in self.nodes:
            result += node.get_property_list(prefix)
        return result

    def __str__(self):
        return ''.join(str(node) for node in self.nodes)

class SubStructNode(AstNode):
    def __init__(self, name, content):
        assert isinstance(name, str)
        assert isinstance(content, AstNode)
        self.name = name
        self.content = content

    def resolve(self, ctx):
        self.content.resolve(ctx)

    def get_property_list(self, prefix):
        return self.content.get_property_list(prefix + self.name + '.')

    def __str__(self):
        return (
            'struct {' +
            ('\n' + code.INDENT).join([''] + str(self.content).splitlines()) +
            '\n} ' + self.name + ';\n')
