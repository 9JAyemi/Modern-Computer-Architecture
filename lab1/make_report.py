from textwrap import wrap

OUT = '/home/ab3394/cornell_tech/Modern-Computer-Architecture/lab1/lab1_report.pdf'
W, H = 612, 792
pages = []
commands = []

def esc(s):
    return s.replace('\\', '\\\\').replace('(', '\\(').replace(')', '\\)')

def text(x, y, s, size=8.5, bold=False, color=(0, 0, 0)):
    font = 'F2' if bold else 'F1'
    commands.append('q %.3f %.3f %.3f rg BT /%s %.1f Tf %.1f %.1f Td (%s) Tj ET Q' %
                    (*color, font, size, x, y, esc(s)))

def line(x1, y1, x2, y2, color=(0.82, 0.82, 0.82), width=0.5):
    commands.append('q %.3f %.3f %.3f RG %.2f w %.2f %.2f m %.2f %.2f l S Q' %
                    (*color, width, x1, y1, x2, y2))

def rect(x, y, w, h, color, stroke=None):
    commands.append('q %.3f %.3f %.3f rg %.2f %.2f %.2f %.2f re f Q' % (*color, x, y, w, h))
    if stroke:
        commands.append('q %.3f %.3f %.3f RG .4 w %.2f %.2f %.2f %.2f re S Q' % (*stroke, x, y, w, h))

def new_page():
    global commands
    if commands:
        pages.append('\n'.join(commands))
    commands = []
    return 750

def paragraph(y, s, width=100, size=8.5, leading=11):
    for part in wrap(s, width):
        if y < 55:
            y = new_page()
        text(45, y, part, size)
        y -= leading
    return y

def heading(y, s):
    if y < 75:
        y = new_page()
    text(45, y, s, 10, True)
    return y - 14

frontend = (0.12, 0.28, 0.53)
bad = (0.89, 0.31, 0.16)
backend = (0.95, 0.76, 0.12)
retiring = (0.33, 0.58, 0.23)

# Page 1: title, implementation, tests, and chart.
y = 752
text(45, y, 'ECE 5755 Lab 1: Kernel Implementation and Top-Down Analysis', 16, True)
y -= 16
text(45, y, 'Intel Xeon Silver 4514Y (Sapphire Rapids); Level-2 profiling', 8.5)
y -= 27

text(45, y, '1. Kernel implementations', 11, True); y -= 17
sections = [
('Convolution.', 'convolution() computes a valid multi-channel 2-D convolution. It allocates one output image per filter, initializes each output with the filter bias, and accumulates products across channels and kernel coordinates. For input side N and kernel side K, the output side is N-K+1. The nested loops perform O(F(N-K+1)^2 C K^2) work.'),
('ReLU.', 'relu() returns x when x is positive and zero otherwise. applyRelu() applies this operation in place across the input array, giving linear work and avoiding a second output allocation.'),
('Linear layer.', 'linear() implements a fully connected layer. Each output starts with its bias and accumulates a dot product between the input vector and one row of weights. Its work is O(I O), where I and O are the input and output sizes.'),
('Matrix multiplication.', 'matmul() checks that the left matrix column count equals the right matrix row count, allocates the result, and uses the conventional three-loop row-column-inner-product algorithm. It returns NULL for incompatible dimensions.'),
('Softmax.', 'softmax() first finds the maximum input, subtracts it before exponentiation for numerical stability, sums the exponentials, and returns log-normalized probabilities. It uses temporary storage for exponentials and a separate output array.'),
]
for title, body in sections:
    text(45, y, title, 8.5, True); y -= 11
    y = paragraph(y, body, 101, 8.2, 10)
    y -= 4

text(45, y, '2. Tests implemented', 11, True); y -= 15
y = paragraph(y, 'The Unity test driver covers convolution, neural-network helpers, functional operations, linear layers, and matrix operations. The tests include ordinary cases and boundary cases such as zero and negative values, large inputs and parameters, softmax normalization, ReLU behavior, square matrix multiplication, and incompatible matrix dimensions. These tests separate numerical correctness from performance measurement and expose indexing, initialization, bias, allocation, and dimension-validation errors.', 101, 8.2, 10)
y -= 12

text(45, y, '3. Top-down analysis profile', 11, True); y -= 14
y = paragraph(y, 'Each standalone kernel was profiled with pmu-tools/toplev.py on an instructional compute node. The chart shows the Level-1 categories reported during Level-2 runs with input size 500. Every bar totals 100 percent.', 101, 8.2, 10)
y -= 10

chart_x, chart_w = 155, 330
values = [
    ('convolution()', [2.3, 2.0, 24.3, 71.4]),
    ('relu()', [28.4, 15.1, 31.0, 25.5]),
    ('linear()', [22.5, 12.3, 31.3, 34.0]),
    ('matmul()', [0.8, 1.2, 19.3, 78.7]),
    ('softmax()', [28.0, 15.1, 31.4, 25.5]),
]
colors = [frontend, bad, backend, retiring]
for p in range(0, 101, 20):
    gx = chart_x + chart_w * p / 100
    line(gx, y - 6, gx, y - 101)
    label = str(p)
    text(gx - (len(label) * 2.1), y - 117, label, 7)
text(chart_x + 102, y - 137, 'Pipeline bottleneck breakdown (%)', 8, True)
for name, vals in values:
    text(45, y - 3, name, 8, True)
    x = chart_x
    for val, col in zip(vals, colors):
        width = chart_w * val / 100
        rect(x, y - 12, width, 17, col)
        x += width
    y -= 28
legend_x = 45
for label, col in zip(['Frontend bound', 'Bad speculation', 'Backend bound', 'Retiring'], colors):
    rect(legend_x, y - 2, 8, 8, col, (0.2, 0.2, 0.2))
    text(legend_x + 12, y - 1, label, 7.5)
    legend_x += 125
y -= 22

text(45, y, 'Kernel', 8, True); text(155, y, 'Frontend', 8, True); text(235, y, 'Bad spec.', 8, True); text(300, y, 'Backend', 8, True); text(385, y, 'Retiring', 8, True)
y -= 12
for name, vals in values:
    text(45, y, name, 8); text(155, y, f'{vals[0]:.1f}%', 8); text(235, y, f'{vals[1]:.1f}%', 8); text(300, y, f'{vals[2]:.1f}%', 8); text(385, y, f'{vals[3]:.1f}%', 8); y -= 11

# Page 2: interpretation and required write-in material.
y = new_page()
text(45, y, '4. Profiling observations and bottlenecks', 11, True); y -= 18
observations = [
('Convolution.', 'The profile is dominated by retiring (71.4%), with backend bound at 24.3%. The Level-2 breakdown is approximately 22.7% core bound and 1.6% memory bound. Explain how the filter/channel loop nest and arithmetic intensity produce this result. Add observations for other input sizes here.'),
('ReLU.', 'ReLU has a balanced profile: 28.4% frontend bound, 15.1% bad speculation, 31.0% backend bound, and 25.5% retiring. Its Level-2 values include 17.1% fetch latency, 11.4% fetch bandwidth, 14.5% branch mispredicts, and 19.2% memory bound. Explain the effects of the conditional branch, input distribution, and memory access.'),
('Linear.', 'The linear layer has 31.3% backend bound and 34.0% retiring. Its Level-2 breakdown is approximately 16.4% memory bound and 14.8% core bound, with 22.5% frontend bound. Relate the result to weight layout, cache behavior, and the dot-product loop order.'),
('Matmul.', 'Matrix multiplication has the highest retiring fraction (78.7%) and small frontend and bad-speculation fractions. Its backend bound value is 19.3%, consisting of approximately 18.5% core bound and 0.8% memory bound. Explain the effect of regular control flow, arithmetic work, and locality in the naive loop order.'),
('Softmax.', 'Softmax has a profile similar to ReLU: 28.0% frontend bound, 15.1% bad speculation, 31.4% backend bound, and 25.5% retiring. Its Level-2 breakdown includes 16.7% fetch latency, 11.4% fetch bandwidth, 14.5% branch mispredicts, and 19.2% memory bound. Discuss multiple passes, temporary arrays, and math-library operations.'),
]
for title, body in observations:
    text(45, y, title, 8.5, True); y -= 11
    y = paragraph(y, body, 101, 8.2, 10)
    y -= 8

text(45, y, '5. Debugging process', 11, True); y -= 15
y = paragraph(y, 'Describe how compiler diagnostics, unit-test failures, targeted tests, memory checking, and profiling output were used to locate and correct bugs. Include one or two concrete failures and the corresponding fixes.', 101, 8.2, 10)
y -= 10
y = paragraph(y, 'Student notes: ____________________________________________________________________________________', 101, 8.2, 10)
y = paragraph(y, '____________________________________________________________________________________________________', 101, 8.2, 10)
y -= 10

text(45, y, '6. Performance improvements', 11, True); y -= 15
y = paragraph(y, 'The top-down data suggests reducing instruction and branch overhead for frontend-bound kernels, improving locality and contiguous allocation for backend-bound kernels, and using compiler vectorization where appropriate. ReLU may benefit from branchless or SIMD execution; linear and convolution may benefit from contiguous layouts and SIMD-friendly inner loops; matmul is a candidate for cache blocking and loop reordering; and softmax could reduce passes and temporary storage while preserving numerical stability.', 101, 8.2, 10)
y -= 8
y = paragraph(y, 'Student notes: Describe the optimizations attempted or proposed and cite the top-down metric motivating each one. Do not claim an improvement without a measurement.', 101, 8.2, 10)
y = paragraph(y, '____________________________________________________________________________________________________', 101, 8.2, 10)
y -= 10

text(45, y, '7. Profiling issues and source disclosure', 11, True); y -= 15
y = paragraph(y, 'Record issues such as login-node perf restrictions, Slurm time limits, CPU affinity, NMI watchdog warnings, counter multiplexing, small-input overhead, failed runs, and how they were resolved. List any external code, documentation, or AI assistance according to the course policy.', 101, 8.2, 10)
y = paragraph(y, 'Student notes: ____________________________________________________________________________________', 101, 8.2, 10)
y = paragraph(y, '____________________________________________________________________________________________________', 101, 8.2, 10)

if commands:
    pages.append('\n'.join(commands))

objects = [
    b'<< /Type /Catalog /Pages 2 0 R >>',
    b'<< /Type /Pages /Kids [3 0 R] /Count %d >>' % len(pages),
]
page_refs = []
obj_no = 3
for content in pages:
    page_refs.append(obj_no)
    obj_no += 2
kids = b' '.join(f'{n} 0 R'.encode() for n in page_refs)
objects[1] = b'<< /Type /Pages /Kids [' + kids + b'] /Count %d >>' % len(pages)
for content in pages:
    page_no = obj_no
    content_no = obj_no + 1
    objects.append(b'<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] /Resources << /Font << /F1 %d 0 R /F2 %d 0 R >> >> /Contents %d 0 R >>' % (len(objects) + 2, len(objects) + 3, content_no))
    data = content.encode('latin-1')
    objects.append(b'<< /Length %d >>\nstream\n%s\nendstream' % (len(data), data))
# Fonts must be the final two objects, referenced by page objects above.
font1 = len(objects) + 1
font2 = len(objects) + 2
# Correct page font references now that object numbers are known.
objects = objects[:2]
for content in pages:
    page_no = len(objects) + 1
    content_no = page_no + 1
    objects.append(b'<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] /Resources << /Font << /F1 %d 0 R /F2 %d 0 R >> >> /Contents %d 0 R >>' % (font1, font2, content_no))
    data = content.encode('latin-1')
    objects.append(b'<< /Length %d >>\nstream\n%s\nendstream' % (len(data), data))
objects.append(b'<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>')
objects.append(b'<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>')

pdf = bytearray(b'%PDF-1.4\n%\xe2\xe3\xcf\xd3\n')
offsets = [0]
for i, obj in enumerate(objects, 1):
    offsets.append(len(pdf))
    pdf.extend(f'{i} 0 obj\n'.encode('ascii')); pdf.extend(obj); pdf.extend(b'\nendobj\n')
xref = len(pdf)
pdf.extend(f'xref\n0 {len(objects)+1}\n0000000000 65535 f \n'.encode('ascii'))
for off in offsets[1:]:
    pdf.extend(f'{off:010d} 00000 n \n'.encode('ascii'))
pdf.extend(f'trailer\n<< /Size {len(objects)+1} /Root 1 0 R >>\nstartxref\n{xref}\n%%EOF\n'.encode('ascii'))
with open(OUT, 'wb') as f:
    f.write(pdf)
print(f'Generated {OUT} ({len(pages)} pages).')
