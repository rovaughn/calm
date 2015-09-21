import sys
import re

data = sys.stdin.read()

csi = '\x1b\\['

colors = ['black', 'red', 'green', 'yellow', 'blue', 'magenta', 'cyan', 'white']

while data:
    m = re.match(csi+r'(?P<row>\d+);(?P<col>\d+)H', data)
    if m:
        print('Move to row %d, col %d' % (int(m.group('row')), int(m.group('col'))))
        data = data[len(m.group(0)):]
        continue

    m = re.match(csi+r'2J', data)
    if m:
        print('Clear screen')
        data = data[len(m.group(0)):]
        continue

    m = re.match(csi+r'2J', data)
    if m:
        print('Clear screen')
        data = data[len(m.group(0)):]
        continue

    m = re.match(csi+r'\?25l', data)
    if m:
        print('Hide cursor')
        data = data[len(m.group(0)):]
        continue

    m = re.match(csi+r'\?25h', data)
    if m:
        print('Show cursor')
        data = data[len(m.group(0)):]
        continue

    m = re.match(csi+r'(?P<sgrs>\d+(;\d+)*)m', data)
    if m:
        for sgr in map(int, m.group('sgrs').split(';')):
            if sgr == 0:
                print('Reset colors')
            elif 30 <= sgr <= 37:
                print('Foreground', colors[sgr - 30])
            elif 40 <= sgr <= 47:
                print('Background', colors[sgr - 40])
            elif 90 <= sgr <= 97:
                print('Bright foreground', colors[sgr - 90])
            elif 100 <= sgr <= 107:
                print('Bright background', colors[sgr - 100])
            else:
                print('Unknown SGR: %d' % sgr)

        data = data[len(m.group(0)):]
        continue

    m = re.match(r'<(?P<comment>.*?)>', data)
    if m:
        print('Comment:', m.group('comment'))
        data = data[len(m.group(0)):]
        continue

    print('Print', repr(data[0]))
    data = data[1:]

