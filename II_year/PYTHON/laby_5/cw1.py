import nbformat

def read_ipynb(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        notebook = nbformat.read(f, as_version=4)
    return notebook

def select_markdown_cells(notebook,markdown_cells):
    for cell in notebook['cells']:
        if cell['cell_type'] == 'markdown':
            markdown_cells.append(cell['source'])            
file_path = 'probny.ipynb'
notebook = read_ipynb(file_path)
markdown_cells = []
markdown_cells = select_markdown_cells(notebook,markdown_cells)
print("length: " + str(len(markdown_cells)))

for idx, cell in enumerate(markdown_cells, start=1):
    print(f"Markdown Cell {idx}:\n{cell}\n{'='*30}")



