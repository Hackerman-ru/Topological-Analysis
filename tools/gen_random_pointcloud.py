import argparse
import random

def generate_points(num_points, min_val, max_val):
    points = []
    for _ in range(num_points):
        x = random.uniform(min_val, max_val)
        y = random.uniform(min_val, max_val)
        z = random.uniform(min_val, max_val)
        points.append((x, y, z))
    return points

def write_off(filename, points):
    with open(filename, 'w') as f:
        f.write("OFF\n")
        f.write(f"{len(points)} 0 0\n")
        for (x, y, z) in points:
            f.write(f"{x} {y} {z}\n")

def main():
    parser = argparse.ArgumentParser(description='Generate random point cloud in OFF format.')
    parser.add_argument('-n', '--num_points', type=int, required=True,
                        help='Number of points to generate')
    parser.add_argument('--min', type=float, default=0.0,
                        help='Minimum coordinate value (default: 0)')
    parser.add_argument('--max', type=float, default=1.0,
                        help='Maximum coordinate value (default: 1)')
    parser.add_argument('-o', '--output', type=str, default='points.off',
                        help='Output filename (default: points.off)')
    
    args = parser.parse_args()
    
    if args.min >= args.max:
        parser.error("Error: 'min' must be less than 'max'")
    
    points = generate_points(args.num_points, args.min, args.max)
    write_off(args.output, points)
    print(f"Generated {args.num_points} points in '{args.output}'")

if __name__ == "__main__":
    main()
