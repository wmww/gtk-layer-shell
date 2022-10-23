'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import logging
from datetime import datetime
import os
from os import path
import re
import mmap
from collections import OrderedDict
import tempfile
import subprocess

import parse
from ast import *
from version import COMBO_FACTOR, Version

logger = logging.getLogger(__name__)

INDENT = '  ' # used for generating code
PROJECT_NAME = 'gtk-layer-shell'

LGPL3_HEADER = '''
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
'''

CODE_EXTENSIONS = set(['.h', '.hpp', '.c', '.cpp', '.cc'])

def typdef_to_struct_name(typedef):
    return '_' + typedef

def get_all_source_files(search_dir):
    result = []
    for item in os.listdir(search_dir):
        p = path.join(search_dir, item)
        if path.isdir(p):
            result += get_all_source_files(p)
        elif path.splitext(p)[1] in CODE_EXTENSIONS:
            result.append(p)
    return result

def remove_headers_from_dir(header_dir):
    logger.info('Clearing header files out of ' + header_dir)
    for item in os.listdir(header_dir):
        p = path.join(header_dir, item)
        if item != 'common.h' and path.splitext(p)[1] == '.h':
            os.remove(p)

def struct_regex_string(struct_name):
    return r'struct\s+' + struct_name + r'\s*\{'

def extract_body(file_content, struct_name):
    halves = re.split(struct_regex_string(struct_name), file_content)
    if len(halves) < 2:
        raise RuntimeError(struct_name + ' not declared in ' + code_path + ' (even though it was detected)')
    if len(halves) > 2:
        raise RuntimeError(struct_name + ' declared multiple times in ' + code_path)
    body = ''
    brace_level = 1
    for c in halves[1]:
        if c == '{':
            brace_level += 1
        elif c == '}':
            brace_level -= 1
        if brace_level <= 0:
            break
        body += c
    return body

def camel_case_to_words(name):
    result = []
    word = ''
    for l in name:
        if l.isupper() and word:
            result.append(word)
            word = ''
        word += l.lower()
    if word:
        result.append(word)
    return result

def c_function(return_type, name, arg_list, body):
    result = ''
    result += return_type.str_left(False)
    result += ' ' + name + '('
    arg_strs = [c_type.str_left(False) + ' ' + name + c_type.str_right(False) for c_type, name in arg_list]
    result += ', '.join(arg_strs)
    result += ')' + return_type.str_right(False) + ' {\n'
    for line in body.strip().splitlines():
        result += INDENT + line + '\n'
    result += '}\n'
    return result

def write_tmp_file(text):
    f = tempfile.NamedTemporaryFile(mode='w', delete=True)
    f.write(text)
    f.flush()
    return f

def diff_between(old, new):
    old_f = write_tmp_file(old)
    new_f = write_tmp_file(new)
    result = subprocess.run(['diff', old_f.name, new_f.name], capture_output=True, text=True)
    code = ''
    for line in result.stdout.splitlines():
        if line.startswith('<'):
            code += '// -' + line[1:] + '\n'
        elif line.startswith('>'):
            code += '// +' + line[1:] + '\n'
    return code

class ResolveContext:
    def __init__(self, project, struct, version):
        self.project = project
        self.struct = struct
        self.version = version

class StructVersion:
    def __init__(self, code_path, project, struct, version):
        with open(code_path, 'r') as f:
            source_code = f.read()
        self.code_path = code_path
        self.first_version = version
        self.last_version = version
        self.struct_name = struct.struct_name
        self.copyright_lines = set(re.findall(r'[Cc]opyright .*(?=\n)', source_code))
        self.body = extract_body(source_code, self.struct_name)
        self.ast = parse.parse_ast(self.body)
        self.ast.resolve(ResolveContext(project, struct, version))

    def get_code_path(self):
        return self.code_path

    def get_property_list(self):
        return self.ast.get_property_list('')

    def version_range_str(self):
        if self.first_version == self.last_version:
            return str(self.first_version)
        else:
            return str(self.first_version) + ' - ' + str(self.last_version)

    def emit_definition(self, generated):
        result = ''
        result += '// Valid for GTK ' + self.version_range_str() + '\n'
        result += 'struct ' + str(self) + '\n{'
        if generated:
            result += (
                '\n' + INDENT +
                ('\n' + INDENT).join(str(self.ast).splitlines()) +
                '\n')
        else:
            result += self.body
        result += '};\n'
        return result

    def is_valid_for(self, version):
        return version >= self.first_version and version <= self.last_version

    def versioned_struct_name(self):
        return self.struct_name + '_' + self.first_version.c_id()

    def __str__(self):
        return self.versioned_struct_name()

    def __eq__(self, other):
        assert isinstance(other, StructVersion)
        return self.ast == other.ast

class Property:
    def __init__(self, struct, c_type, name, version_ids):
        assert isinstance(struct, Struct)
        assert isinstance(c_type, CType)
        assert isinstance(name, str)
        assert isinstance(version_ids, list)
        for i in version_ids:
            assert isinstance(i, bool) # If this property is supported for each version ID
        self.struct = struct
        self.c_type = c_type
        self.name = name
        self.version_ids = version_ids

    def all_versions_supported(self):
        for i in self.version_ids:
            if not i:
                return False
        return True

    def get_id_name(self):
        return self.name.replace('.', '_')

    def get_fn_name(self, action, suffix):
        result = ''
        result += '_'.join(camel_case_to_words(self.struct.typedef))
        result += '_priv_'
        if action:
            result += action + '_'
        result += self.get_id_name()
        if suffix:
            result += '_' + suffix
        return result

    def emit_version_id_switch(self, on_supported, on_unsupported):
        result = ''
        result += 'switch (' + self.struct.get_version_id_fn_name() + '()) {\n'
        for i, supported in enumerate(self.version_ids):
            result += INDENT + 'case ' + str(i) + ': '
            type_name = 'struct ' + self.struct.versions[i].versioned_struct_name()
            if supported:
                result += on_supported(type_name)
            else:
                result += on_unsupported(type_name)
            result += '\n'
        result += INDENT + 'default: g_error("Invalid version ID"); g_abort();\n'
        result += '}'
        return result

    def emit_ptr_getter(self):
        ret_type = PtrType(self.c_type)
        suffix = 'ptr' if self.all_versions_supported() else 'ptr_or_null'
        fn_name = self.get_fn_name('get', suffix)
        arg_list = [(self.struct.get_ptr_type(), 'self')]
        body = self.emit_version_id_switch(
            lambda type_name: 'return (' + str(ret_type) + ')&((' + type_name + '*)self)->' + self.name + ';',
            lambda type_name: 'return NULL;')
        return c_function(ret_type, fn_name, arg_list, body)

    def emit_is_supported(self):
        ret_type = StdType('gboolean')
        fn_name = self.get_fn_name('get', 'supported')
        body = self.emit_version_id_switch(
            lambda type_name: 'return TRUE;',
            lambda type_name: 'return FALSE;')
        return c_function(ret_type, fn_name, [], body)

    def emit_not_supported_error(self, type_name):
        msg = '"' + self.struct.typedef + '::' + self.name + ' not supported on this GTK"'
        return 'g_error(' + msg + '); g_abort();'

    def emit_getter(self):
        ret_type = self.c_type
        suffix = '' if self.all_versions_supported() else 'or_abort'
        fn_name = self.get_fn_name('get', suffix)
        arg_list = [(self.struct.get_ptr_type(), 'self')]
        body = self.emit_version_id_switch(
            lambda type_name: 'return ((' + type_name + '*)self)->' + self.name + ';',
            self.emit_not_supported_error)
        return c_function(ret_type, fn_name, arg_list, body)

    def emit_setter(self):
        ret_type = StdType('void')
        suffix = '' if self.all_versions_supported() else 'or_abort'
        fn_name = self.get_fn_name('set', suffix)
        arg_list = [(self.struct.get_ptr_type(), 'self'), (self.c_type, self.get_id_name())]
        body = self.emit_version_id_switch(
            lambda type_name: '((' + type_name + '*)self)->' + self.name + ' = ' + self.get_id_name() + '; break;',
            self.emit_not_supported_error)
        return c_function(ret_type, fn_name, arg_list, body)

    def emit_functions(self):
        result = ''
        result += '// ' + self.struct.typedef + '::' + self.name + '\n\n'
        if not self.all_versions_supported():
            result += self.emit_is_supported()
            result += '\n'
        if isinstance(self.c_type, CustomType) or isinstance(self.c_type, ArrayType):
            result += self.emit_ptr_getter()
        else:
            result += self.emit_getter()
            result += '\n'
            result += self.emit_setter()
        return result

class Struct:
    def __init__(self, typedef):
        self.typedef = typedef
        self.struct_name = typdef_to_struct_name(typedef)
        self.versions = []
        self.supported_versions = []
        self.copyright_lines = set()
        self.search_regex = re.compile(bytes(struct_regex_string(self.struct_name), 'utf-8'))

    def lookup_version(self, version):
        for i in self.versions:
            if i.is_valid_for(version):
                return i

    def get_code_path(self):
        if self.versions:
            return self.versions[-1].get_code_path()
        else:
            return None

    def get_ptr_type(self):
        return PtrType(CustomType(self.typedef))

    def get_version_id_fn_name(self):
        return '_'.join(camel_case_to_words(self.typedef)) + '_priv_get_version_id'

    def header_name(self):
        return '_'.join(camel_case_to_words(self.typedef)) + '_priv.h'

    def macro_name(self):
        return self.header_name().replace('.', '_').upper()

    def add_version(self, new):
        self.versions.append(new)
        self.supported_versions.append(new.first_version)
        self.copyright_lines = self.copyright_lines.union(new.copyright_lines)

    # Returns if any changes were made
    def simplify(self):
        new_versions = []
        dropped_versions = 0
        for v in self.versions:
            if new_versions and new_versions[-1] == v:
                dropped_versions += 1
                new_versions[-1].last_version = v.last_version
            else:
                new_versions.append(v)
        if dropped_versions > 0:
            logger.info('Found ' + str(dropped_versions) + ' unnecessary versions of ' + self.typedef);
        self.versions = new_versions
        return dropped_versions > 0

    def setup_properties(self):
        prop_dict = OrderedDict()
        for i, v in enumerate(self.versions):
            for c_type, name in v.get_property_list():
                if name in prop_dict and not (prop_dict[name][0] == c_type):
                    logger.warning(
                        'Property ' + name +
                        ' changes type from ' + str(prop_dict[name][0]) +
                        ' to ' + str(c_type) +
                        ', ignoring old type')
                    del prop_dict[name]
                if name in prop_dict:
                    assert prop_dict[name][0] == c_type
                    prop_dict[name][1].add(i)
                else:
                    prop_dict[name] = (c_type, set([i]))
        self.properties = []
        for name, (c_type, supported_ids) in prop_dict.items():
            versions_ids = [bool(i in supported_ids) for i in range(len(self.versions))]
            self.properties.append(Property(self, c_type, name, versions_ids))

    def emit_get_version_id_fn(self):
        return_type = StdType('int')
        fn_name = self.get_version_id_fn_name()
        body = ''
        body += 'static int version_id = -1;\n'
        body += '\n'
        body += 'if (version_id == -1) {\n'
        body += INDENT + 'gtk_priv_assert_gtk_version_valid();'
        body += '\n'
        body += INDENT + 'int combo = gtk_get_minor_version() * 1000 + gtk_get_micro_version();\n'
        body += '\n'
        body += INDENT + 'switch (combo) {\n'
        for v in self.supported_versions:
            if v.is_released():
                body += INDENT * 2 + 'case ' + str(v.get_combo()) + ':\n'
        body += INDENT * 3 + 'break;\n'
        body += '\n'
        body += INDENT * 2 + 'default:\n'
        body += INDENT * 3 + 'gtk_priv_warn_gtk_version_may_be_unsupported();\n'
        body += INDENT + '}\n'
        body += '\n'
        body += INDENT
        for i in range(len(self.versions) - 1, -1, -1):
            if i > 0:
                body += 'if (combo >= ' + str(self.versions[i].first_version.get_combo()) + ') '
            body += '{\n'
            body += INDENT * 2 + 'version_id = ' + str(i) + ';\n'
            body += INDENT + '}'
            if i > 0:
                body += ' else '
        body += '\n'
        body += '}\n'
        body += '\n'
        body += 'return version_id;\n'
        return c_function(return_type, fn_name, [], body)

    def emit_header(self, generated):
        result = ''
        result += '/* AUTOGENERATED, DO NOT EDIT DIRECTLY\n'
        result += ' * See gtk-priv/README.md for more information\n'
        result += ' *\n'
        result += ' * This file is part of ' + PROJECT_NAME + '\n'
        result += ' *\n'
        me = path.relpath(__file__, path.dirname(path.dirname(path.dirname(__file__))))
        my_copyright_line = 'Copyright © ' + str(datetime.now().year) + ' ' + me
        for line in list(self.copyright_lines) + [my_copyright_line]:
            result += ' * ' + line + '\n'
        result += ' *'
        result += '\n * '.join(LGPL3_HEADER.splitlines())
        result += '\n */\n'
        result += '\n'
        result += '#ifndef ' + self.macro_name() + '\n'
        result += '#define ' + self.macro_name() + '\n'
        result += '\n'
        result += '#include "common.h"\n'
        result += '\n'
        result += 'typedef struct ' + self.struct_name + ' ' + self.typedef + ';\n'
        result += '\n'
        prev_definition = None
        for i, struct in enumerate(self.versions):
            result += '// Version ID ' + str(i) + '\n'
            definition = struct.emit_definition(generated)
            if prev_definition:
                result += '// Diff from previous version:\n'
                result += '\n'.join(diff_between(prev_definition, definition).splitlines()[4:]) + '\n'
            prev_definition = definition
            result += definition
            result += '\n'
        result += '// For internal use only\n'
        result += self.emit_get_version_id_fn()
        result += '\n'
        for p in self.properties:
            result += p.emit_functions()
            result += '\n'
        result += '#endif // ' + self.macro_name() + '\n'
        return result

def file_contains_byte_regex(source_file, regex):
    try:
        with open(source_file) as f:
            with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as s:
                return bool(regex.search(s))
    except (ValueError, FileNotFoundError):
        return False

class Project:
    def __init__(self, repo_dir, typedef_names):
        self.repo_dir = repo_dir
        structs = [Struct(typedef) for typedef in typedef_names]
        self.typedefs = {struct.typedef: struct for struct in structs}
        self.struct_names = {struct.struct_name: struct for struct in structs}

    def lookup_struct_name(self, struct_name):
        return self.struct_names.get(struct_name)

    def lookup_typedef(self, typedef):
        return self.typedefs.get(typedef)

    def update(self, version):
        source_files = None
        for typedef, struct in self.typedefs.items():
            code_path = struct.get_code_path()
            search_regex = struct.search_regex
            if not code_path or not file_contains_byte_regex(code_path, search_regex):
                if source_files is None:
                    source_files = get_all_source_files(self.repo_dir)
                files = set()
                for source_file in source_files:
                    if file_contains_byte_regex(source_file, search_regex):
                        files.add(source_file)
                if not files:
                    raise RuntimeError('Could not find ' + typedef + ' in ' + str(version))
                if len(files) > 1:
                    raise RuntimeError(typedef + ' implemented multiple places: ' + str(files))
                code_path = list(files)[0]
            struct_version = StructVersion(code_path, self, struct, version)
            struct.add_version(struct_version)

    def simplify(self):
        i = 1;
        while True:
            logger.info('Detecting identical versions, round ' + str(i))
            i += 1
            made_change = False
            for _, struct in self.typedefs.items():
                if struct.simplify():
                    made_change = True
            if not made_change:
                break
        for _, struct in self.typedefs.items():
            struct.setup_properties()

    def write(self, output_dir):
        remove_headers_from_dir(output_dir)
        for typedef, struct in self.typedefs.items():
            output_path = path.join(output_dir, struct.header_name())
            logger.info('Writing ' + str(len(struct.versions)) + ' versions of ' + typedef + ' to ' + output_path)
            with open(output_path, "w") as f:
                f.write(struct.emit_header(True))
