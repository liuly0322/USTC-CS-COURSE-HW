import csv
import subprocess


def compile_and_run_program(n, block_size):
    # 编译程序并传递宏定义 BLOCK
    command = ['nvcc', '-O2', '-o', 'gpu_block',
               '-DBLOCK='+str(block_size), 'src_gpu/gpu_block.cu']
    subprocess.run(command)

    # 运行程序并获取输出
    command = ['./gpu_block', str(n)]
    result = subprocess.run(command, capture_output=True, text=True)
    output = result.stdout.strip()
    # 从输出中提取时间
    time = float(output.split(': ')[-1].split(' ')[0])
    return time


n_values = [256, 512, 1024, 2048]
block_sizes = [4, 8, 16, 32]

# 生成 Markdown 表格的头部
table_header = "| N | 4 | 8 | 16 | 32 |\n"
table_header += "| --- | --- | --- | --- | --- |\n"

# 生成 Markdown 表格的内容
table_content = ""
for n in n_values:
    row = f"| {n} "
    for block_size in block_sizes:
        elapsed_time = compile_and_run_program(n, block_size)
        row += f"| {elapsed_time} "
    row += "|\n"
    table_content += row

# 将表格写入 Markdown 文件
with open('result.md', 'w') as file:
    file.write(table_header)
    file.write(table_content)
