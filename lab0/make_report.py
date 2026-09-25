from textwrap import wrap

PAGE_W, PAGE_H = 612, 792
commands = []

def esc(value):
    return value.replace('\\', '\\\\').replace('(', '\\(').replace(')', '\\)')

def text(x, y, value, size=9, font='F1', color=(0, 0, 0)):
    commands.append('q %.3f %.3f %.3f rg BT /%s %.1f Tf %.1f %.1f Td (%s) Tj ET Q' %
                    (*color, font, size, x, y, esc(value)))

def text_center(x_center, y, value, size=8, font='F1', color=(0, 0, 0)):
    approx_w = len(value) * (size * 0.52)
    x = x_center - (approx_w / 2.0)
    text(x, y, value, size, font, color)

def rect_fill(x, y, w, h, color):
    commands.append('q %.3f %.3f %.3f rg %.2f %.2f %.2f %.2f re f Q' %
                    (*color, x, y, w, h))

def rect_stroke(x, y, w, h, color=(0, 0, 0), width=0.5):
    commands.append('q %.3f %.3f %.3f RG %.2f w %.2f %.2f %.2f %.2f re S Q' %
                    (*color, width, x, y, w, h))

def line(x1, y1, x2, y2, color=(0.8, 0.8, 0.8), width=0.5):
    commands.append('q %.3f %.3f %.3f RG %.2f w %.2f %.2f m %.2f %.2f l S Q' %
                    (*color, width, x1, y1, x2, y2))

def paragraph(y, value, width_chars=101, size=8.5, leading=11):
    for line_text in wrap(value, width_chars):
        text(45, y, line_text, size)
        y -= leading
    return y

black = (0, 0, 0)
frontend = (0.12, 0.28, 0.53)
bad = (0.89, 0.31, 0.16)
backend = (0.95, 0.76, 0.12)
retiring = (0.33, 0.58, 0.23)

text(45, 752, 'ECE 5755 Lab 0: Sorting and Top-Down Analysis', 16, 'F2')
text(45, 736, '5,000-element input on Intel Xeon Silver 4514Y (Sapphire Rapids)', 8.5)

y = 712
text(45, y, 'Implementation.', 9, 'F2')
y -= 13
y = paragraph(y, 'I implemented an in-place, recursive top-down mergesort in mysort.c. Each recursive call divides the array, sorts both halves, and merges them using temporary left and right arrays; the <= comparison preserves stability. The wrapper returns the same sorted input pointer. This has O(n log n) time complexity and O(n) temporary merge storage. The output passed the supplied sortedness check. The merge-sort structure was adapted from GeeksforGeeks, "Merge Sort," https://www.geeksforgeeks.org/dsa/merge-sort/ (accessed September 8, 2026).', 101)

y -= 6
text(45, y, 'Top-down profiling.', 9, 'F2')
y -= 13
y = paragraph(y, 'I ran pmu-tools/toplev.py with -l1 -v --no-desc --force-cpu spr on the ECE 5755 compute node. The horizontal bar chart below compares the Level-1 pipeline bottleneck breakdown (%) for both implementations.', 101)

chart_x = 145
chart_w = 325
chart_top = y - 10
chart_h = 80
chart_bot = chart_top - chart_h

pcts = [0, 20, 40, 60, 80, 100]
grid_x_coords = []
for p in pcts:
    gx = chart_x + (chart_w * p / 100.0)
    grid_x_coords.append(gx)
    line(gx, chart_bot, gx, chart_top, color=(0.82, 0.82, 0.82), width=0.5)

rect_stroke(chart_x, chart_bot, chart_w, chart_h, color=(0.5, 0.5, 0.5), width=0.75)

data = [
    ('mysort (input_5000)', [25.4, 23.3, 18.7, 32.7], chart_bot + 48, 22),
    ('bubble (input_5000)', [2.3, 6.7, 19.5, 71.5], chart_bot + 12, 22)
]

bar_colors = [frontend, bad, backend, retiring]

for label, values, bar_y, bar_height in data:
    text(45, bar_y + 7, label, 8, 'F2')
    curr_x = chart_x
    for val, col in zip(values, bar_colors):
        seg_w = chart_w * (val / 100.0)
        rect_fill(curr_x, bar_y, seg_w, bar_height, col)
        curr_x += seg_w

for p, gx in zip(pcts, grid_x_coords):
    line(gx, chart_bot - 3, gx, chart_bot, color=(0.3, 0.3, 0.3), width=0.75)
    text_center(gx, chart_bot - 12, str(p), size=7.5, font='F1')

text_center(chart_x + (chart_w / 2.0), chart_bot - 24, 'Pipeline bottleneck breakdown (%)', size=8, font='F2')

legend_items = [
    ('Frontend bound', frontend),
    ('Bad speculation', bad),
    ('Backend bound', backend),
    ('Retiring', retiring)
]
leg_x = chart_x + chart_w + 12
leg_y = chart_top - 12
for leg_label, leg_col in legend_items:
    rect_fill(leg_x, leg_y, 8, 8, leg_col)
    rect_stroke(leg_x, leg_y, 8, 8, color=(0.2, 0.2, 0.2), width=0.3)
    text(leg_x + 12, leg_y + 1, leg_label, 7.5, 'F1')
    leg_y -= 18

y = chart_bot - 38

rows = [
    ('Implementation', 'Frontend', 'Bad spec.', 'Backend', 'Retiring'),
    ('Bubble sort', '2.3%', '6.7%', '19.5%', '71.5%'),
    ('Mergesort', '25.4%', '23.3%', '18.7%', '32.7%'),
]
col_x = [45, 170, 255, 335, 415]
for row_index, row in enumerate(rows):
    for cx, value in zip(col_x, row):
        text(cx, y, value, 8, 'F2' if row_index == 0 else 'F1')
    y -= 13

y -= 6
text(45, y, 'Observations.', 9, 'F2')
y -= 13
y = paragraph(y, 'Bubble sort was dominated by retiring (71.5%), indicating that most measured slots completed useful instructions. Mergesort shifted toward frontend bound (25.4%) and bad speculation (23.3%), with only 32.7% retiring. Its backend-bound percentage was similar to bubble sort (18.7% versus 19.5%). The different profile is consistent with mergesort recursion, merge control flow, and temporary-array accesses, although the percentages describe pipeline behavior rather than algorithmic complexity alone. Both runs reported 100% counter multiplexing coverage.', 101)

y -= 6
text(45, y, 'Issues.', 9, 'F2')
y -= 13
y = paragraph(y, 'Profiling from the login node was blocked by perf_event_paranoid=4; running inside an srun allocation on the instructional compute node resolved access. toplev also warned about the NMI watchdog and could not open an uncore file, but it still produced the core Level-1 results. The final packaged source leaves the verification call commented out as required for profiling.', 101)

content = '\n'.join(commands).encode('latin-1')
objects = [
    b'<< /Type /Catalog /Pages 2 0 R >>',
    b'<< /Type /Pages /Kids [3 0 R] /Count 1 >>',
    b'<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] /Resources << /Font << /F1 4 0 R /F2 5 0 R >> >> /Contents 6 0 R >>',
    b'<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>',
    b'<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>',
    b'<< /Length %d >>\nstream\n%s\nendstream' % (len(content), content),
]
pdf = bytearray(b'%PDF-1.4\n%\xe2\xe3\xcf\xd3\n')
offsets = [0]
for number, obj in enumerate(objects, 1):
    offsets.append(len(pdf))
    pdf.extend(('%d 0 obj\n' % number).encode('ascii'))
    pdf.extend(obj)
    pdf.extend(b'\nendobj\n')
xref = len(pdf)
pdf.extend(('xref\n0 %d\n0000000000 65535 f \n' % (len(objects) + 1)).encode('ascii'))
for offset in offsets[1:]:
    pdf.extend(('%010d 00000 n \n' % offset).encode('ascii'))
pdf.extend(('trailer\n<< /Size %d /Root 1 0 R >>\nstartxref\n%d\n%%%%EOF\n' % (len(objects) + 1, xref)).encode('ascii'))
with open('/home/ab3394/lab0.pdf', 'wb') as report:
    report.write(pdf)
print('lab0.pdf generated successfully.')
print('lab0.pdf generated successfully.')
