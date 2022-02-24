"""A wrapper script around clang-format, suitable for linting multiple files
and to use for continuous integration.

This is an alternative API for the clang-format command line.
It runs over multiple files and directories in parallel.

"""
import argparse
import io
import errno
import os
import subprocess
import sys


DEFAULT_EXTENSIONS = 'c,h,C,H,cpp,hpp,cc,hh,c++,h++,cxx,hxx'
DEFAULT_CLANG_FORMAT_IGNORE = '.clang-format-ignore'


def excludes_from_file(ignore_file):
    excludes = []
    try:
        with io.open(ignore_file, 'r') as f:
            for line in f:
                if line.startswith('#'):
                    # ignore comments
                    continue
                pattern = line.rstrip()
                if not pattern:
                    # allow empty lines
                    continue
                excludes.append(pattern)
    except EnvironmentError as e:
        if e.errno != errno.ENOENT:
            raise
    return excludes;

def list_files(files, recursive=False, extensions=None, exclude=None):
    if extensions is None:
        extensions = []
    if exclude is None:
        exclude = []

    out = []
    for file in files:
        if recursive and os.path.isdir(file):
            for dirpath, dnames, fnames in os.walk(file):
                fpaths = [os.path.join(dirpath, fname) for fname in fnames]
                for pattern in exclude:
                    # os.walk() supports trimming down the dnames list
                    # by modifying it in-place,
                    # to avoid unnecessary directory listings.
                    dnames[:] = [
                        x for x in dnames
                        if
                        not fnmatch.fnmatch(os.path.join(dirpath, x), pattern)
                    ]
                    fpaths = [
                        x for x in fpaths if not fnmatch.fnmatch(x, pattern)
                    ]
                for f in fpaths:
                    ext = os.path.splitext(f)[1][1:]
                    if ext in extensions:
                        out.append(f)
        else:
            out.append(file)
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '-r',
        '--recursive',
        action='store_true',
        help='run recursively over directories')
    parser.add_argument('files', metavar='file', nargs='+')
    parser.add_argument(
        '-e',
        '--exclude',
        metavar='PATTERN',
        action='append',
        default=[],
        help='exclude paths matching the given glob-like pattern(s)'
        ' from recursive search')

    args = parser.parse_args()

    excludes = excludes_from_file(DEFAULT_CLANG_FORMAT_IGNORE)
    excludes.extend(args.exclude)

    # Workaround for CCS not picking up the full path in Unix systems
    if os.name == 'posix':
        os.environ['PATH'] = '/usr/local/bin:' + os.environ['PATH']
 
    # Check for Clang version
    try:
        process = subprocess.Popen(['clang-format', '--version'], stdout=subprocess.PIPE)
    except FileNotFoundError:
        print("Clang format not installed. See instructions here: https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/wikis/Clang-Format")
        return -1

    version = process.stdout.readline()
    version_split = str(version).split(' ')
    if (version_split[2] != "10.0.0"):
        print("Clang format version unsupported, please use 10.0.0. See instructions here: https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/wikis/Clang-Format")
        return -1

    files = list_files(
        args.files,
        recursive=args.recursive,
        exclude=excludes,
        extensions=DEFAULT_EXTENSIONS.split(','))

    if not files:
        return

    processes = []
    for file in files:
        invocation = ['clang-format', '-i', file]
        proc = subprocess.Popen(
            invocation,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True)
        processes.append(proc)
            
    for proc in processes:
        proc.wait()
    return 0


if __name__ == '__main__':
    sys.exit(main())
