#!/usr/bin/env python3
import subprocess
import os
import platform
import argparse
import datetime
import sys
import shlex 

# --- Dependency Guard ---
try:
    import svgutils.compose as sg
except ImportError:
    print("\n❌ Error: 'svgutils' library not found.")
    print("Please run: python3 -m pip install svgutils --break-system-packages\n")
    sys.exit(1)

def get_input_interactively(arg_value, description):
    while not arg_value:
        arg_value = input(f"Please enter {description}: ").strip()
    if "Output" in description and not arg_value.lower().endswith(".svg"):
        arg_value += ".svg"
    return arg_value

def export_and_stitch(project_name, output_filename, scale, should_print, gap_mm, double_mode, cal_x, cal_y, verbose, positive):
    cwd = os.getcwd()
    
    clean_project = project_name if project_name.endswith(".kicad_pcb") else f"{project_name}.kicad_pcb"
    pcb_file = os.path.abspath(os.path.join(cwd, clean_project))
    
    if not output_filename.lower().endswith(".svg"):
        output_filename += ".svg"
    output_path = os.path.abspath(os.path.join(cwd, output_filename))
    
    temp_f, temp_b = os.path.join(cwd, "temp_f.svg"), os.path.join(cwd, "temp_b.svg")

    if not os.path.exists(pcb_file):
        pcb_file = os.path.abspath(os.path.join(cwd, project_name, clean_project))
        print(f"\nSearching for project file in the same-named project folder")
        if not os.path.exists(pcb_file):
            print(f"\n❌ Error: Could not find '{clean_project}'")
            sys.exit(1)

    kicad_cli = "/Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli" if platform.system() == "Darwin" else "kicad-cli"

    common_args = [
        kicad_cli, "pcb", "export", "svg", 
        "--black-and-white", "--mode-single", 
        "--exclude-drawing-sheet", "--fit-page-to-board",
        "--drill-shape-opt", "1"
    ]
    if not positive: common_args.append("--negative")

    def run_kicad_cmd(extra_args, target_file):
        cmd = common_args + extra_args + ["-o", target_file, pcb_file]
        
        # Determine output redirection based on verbose flag
        # If not verbose, redirect stdout and stderr to DEVNULL
        stdout_dest = None if verbose else subprocess.DEVNULL
        stderr_dest = None if verbose else subprocess.DEVNULL

        if verbose:
            print(f"DEBUG CLI: {' '.join(shlex.quote(c) for c in cmd)}")
        
        subprocess.run(cmd, check=True, stdout=stdout_dest, stderr=stderr_dest)

    try:
        print(f"⏳ Exporting layers from {clean_project}...")
        run_kicad_cmd(["--layers", "F.Cu,Edge.Cuts", "--mirror"], temp_f)
        run_kicad_cmd(["--layers", "B.Cu,Edge.Cuts"], temp_b)
    except Exception as e:
        print(f"❌ KiCad Error: {e}")
        return

    # --- COMPOSE SCALING LOGIC (NO DEEPCOPY) ---
    svg_fcu = sg.SVG(temp_f)
    svg_bcu = sg.SVG(temp_b)

    Vgap_px = sg.Unit(f"{gap_mm}mm").to('px').value
    Hgap_px = sg.Unit(f"{gap_mm}mm").to('px').value
    LabelVgap_px = sg.Unit("10mm").to('px').value



    OutputSVG_width_px = max(svg_fcu.width, svg_bcu.width)
    if double_mode:
        OutputSVG_width_px *= 2
        OutputSVG_width_px += Hgap_px

    OutputSVG_height_px = svg_fcu.height + svg_bcu.height + Vgap_px + LabelVgap_px

    Bcu_Vpos_px = svg_fcu.height + Vgap_px
    Label_Vpos_px = Bcu_Vpos_px + svg_bcu.height + LabelVgap_px

    print(f"F.cu: {sg.Unit(svg_fcu.width).to('mm')} x {sg.Unit(svg_fcu.height).to('mm')} \r\nB.cu: {sg.Unit(svg_bcu.width).to('mm')} x {sg.Unit(svg_bcu.height).to('mm')}")

    # Label & Metadata
    ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
    svg_label_text = f"File: {os.path.basename(output_path)} | Scale: {scale}:1 | Date: {ts}"
    svg_label = sg.Text(svg_label_text, 
                        sg.Unit(OutputSVG_width_px).to('mm').value, 
                        sg.Unit(Label_Vpos_px).to('mm').value, 
                        size=3, font="monospace", anchor="end", color="red")

    OutputSVG__params = []

    if double_mode:
        OutputSVG_col2_pos_px = max(svg_fcu.width, svg_bcu.width) + Hgap_px
        svg_bcu = svg_bcu.move(0, sg.Unit(Bcu_Vpos_px).to('mm').value)
        OutputSVG__params = [
            svg_fcu,
            sg.Element(svg_fcu.copy()).move(sg.Unit(OutputSVG_col2_pos_px).to('mm').value, 0),
            svg_bcu,
            sg.Element(svg_bcu.copy()).move(sg.Unit(OutputSVG_col2_pos_px).to('mm').value, 0), 
            svg_label
            ]
    else:
        OutputSVG__params = [
            svg_fcu,
            svg_bcu.move(0, sg.Unit(Bcu_Vpos_px).to('mm').value),
            svg_label
        ]


    sg.Figure(
        f"{sg.Unit(OutputSVG_width_px).to('mm')}",
        f"{sg.Unit(OutputSVG_height_px).to('mm')}",
        *OutputSVG__params,
     ).save(output_path)

    print(f"\nRemoving temporary files...")
    os.remove(temp_f); os.remove(temp_b)
    print(f"\n✅ Production File Created: {output_path}")

    if should_print and platform.system() == "Darwin":
        subprocess.run(["open", "-a", "GIMP", output_path])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="KiCad Fabricator: Clean Composition Workflow")
    parser.add_argument("project", nargs="?")
    parser.add_argument("output", nargs="?")
    parser.add_argument("--scale", type=float, default=1.0)
    parser.add_argument("--cal-x", type=float, default=1.0)
    parser.add_argument("--cal-y", type=float, default=1.0)
    parser.add_argument("--print", action="store_true")
    parser.add_argument("--gap",type=float,default=10)
    #parser.add_argument("--a4", action="store_true")
    parser.add_argument("--double", action="store_true")
    parser.add_argument("--verbose", action="store_true")
    parser.add_argument("--positive", action="store_true")

    args = parser.parse_args()
    
    if len(sys.argv) == 1:
        parser.print_help()
        print("\n--- Interactive Input ---")

    proj = get_input_interactively(args.project, "Project Name (.kicad_pcb)")
    out = get_input_interactively(args.output, "Output Filename")
    
    export_and_stitch(proj, out, args.scale, args.print, args.gap, args.double, args.cal_x, args.cal_y, args.verbose, args.positive)