import numpy as np
from math import sqrt
import argparse
# Create the parser
parser = argparse.ArgumentParser(description="Get io_file generation parameters")
is_vectorization_activated = True
parser.add_argument("--slot_count", required=True,type=int,help="Slot_count")
# Parse arguments
args = parser.parse_args()
slot_count = args.slot_count 
