#!/usr/bin/env python3
"""检查 main/ 下 C 代码字符串里有没有中文字库（font_fusion12）没收录的字。

用法：python3 tools/check_glyphs.py            # 检查 main/apps/
      python3 tools/check_glyphs.py main/xxx.c  # 检查指定文件

没收录的字在设备上会显示成方块，换成近义的常用字即可。
字表在 docs/watcha/fusion12_chars.txt。
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHARS = os.path.join(ROOT, 'docs', 'watcha', 'fusion12_chars.txt')


def main():
    have = set(open(CHARS, encoding='utf-8').read()) | {chr(c) for c in range(0x20, 0x7f)}
    paths = sys.argv[1:] or [os.path.join(ROOT, 'main', 'apps', f)
                             for f in sorted(os.listdir(os.path.join(ROOT, 'main', 'apps'))) if f.endswith('.c')]
    bad = 0
    for p in paths:
        src = open(p, encoding='utf-8').read()
        src = re.sub(r'/\*.*?\*/', lambda m: '\n' * m.group(0).count('\n'), src, flags=re.S)   # 去掉块注释，保留行号
        for lineno, line in enumerate(src.split('\n'), 1):
            if line.lstrip().startswith('//'):
                continue
            for s in re.findall(r'"((?:[^"\\]|\\.)*)"', line):
                miss = sorted({ch for ch in s if ord(ch) > 0x7f and ch not in have})
                if miss:
                    bad += 1
                    print(f'{os.path.relpath(p, ROOT)}:{lineno}: 字库没有 {" ".join(miss)}  ->  "{s}"')
    if bad:
        print(f'\n共 {bad} 处会显示成方块，请换字。')
        sys.exit(1)
    print('OK：所有中文都在字库里。')


if __name__ == '__main__':
    main()
