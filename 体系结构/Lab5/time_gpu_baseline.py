import subprocess


def run_program_and_get_time(n, block_size):
    command = ['./gpu_baseline', str(n), str(block_size)]
    result = subprocess.run(command, capture_output=True, text=True)
    output = result.stdout.strip()
    # 从输出中提取时间
    time = float(output.split(': ')[-1].split(' ')[0])
    return time


n_values = [256, 512, 1024, 2048]
block_sizes = [4, 8, 16, 32]

# 生成Markdown表格的头部
table_header = "| N | 4 | 8 | 16 | 32 |\n"
table_header += "| --- | --- | --- | --- | --- |\n"

# 生成Markdown表格的内容
table_content = ""
for n in n_values:
    row = f"| {n} "
    for block_size in block_sizes:
        elapsed_time = run_program_and_get_time(n, block_size)
        row += f"| {elapsed_time} "
    row += "|\n"
    table_content += row

# 将表格写入Markdown文件
with open('result.md', 'w') as file:
    file.write(table_header)
    file.write(table_content)
