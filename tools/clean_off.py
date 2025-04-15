def clean_off_file(input_path, output_path, precision=3):
    """Очищает .OFF файл от дубликатов точек с заданной точностью"""
    
    with open(input_path, 'r') as f:
        lines = [line.strip() for line in f.readlines()]

    if not lines or lines[0] != 'OFF':
        raise ValueError("Invalid OFF file format")

    header = list(map(int, lines[1].split()))
    num_vertices, num_faces, num_edges = header

    vertices = []
    for line in lines[2:2+num_vertices]:
        parts = line.split()
        if len(parts) != 3:
            continue
        try:
            vertex = tuple(map(float, parts))
            vertices.append(vertex)
        except ValueError:
            continue

    seen = set()
    unique_vertices = []
    for v in vertices:
        key = tuple(round(coord, precision) for coord in v)
        if key not in seen:
            seen.add(key)
            unique_vertices.append(v)

    new_header = f"OFF\n{len(unique_vertices)} {num_faces} {num_edges}\n"
    new_vertices = [" ".join(map(str, v)) + "\n" for v in unique_vertices]

    with open(output_path, 'w') as f:
        f.write(new_header)
        f.writelines(new_vertices)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python clean_off.py input.off output.off")
        sys.exit(1)
    
    clean_off_file(sys.argv[1], sys.argv[2])
    print(f"Cleaned file saved to {sys.argv[2]}")
