#!/usr/bin/env python3

import os
import sys
import shutil
import subprocess
import random
import glob
import time
import threading
from pathlib import Path

FOLDER_NAME = sys.argv[1] if len(sys.argv) > 1 else None

if FOLDER_NAME is None:
    print("Usage: python alldown.py <folder> <prefix> (<pathlength> <processors>)")
    sys.exit(1)


PREFIX = sys.argv[2] if len(sys.argv)>2 else None

if PREFIX is None:
    print("Usage: python alldown.py <folder> <prefix> (<pathlength> <processors>)")
    sys.exit(1)


PATHLENGTH = sys.argv[3] if len(sys.argv)>3 else '10000000'


PARALLEL_INSTANCES = int(sys.argv[4]) if len(sys.argv)>4 else 32


TOTAL_NO_REDUCTION_FLIPS_THRESHOLD = 1024 # Total ./flip calls (across all threads) that yield no reduction before moving to next size.
TOTAL_REDUCTIONS_FOUND_THRESHOLD = 256 # Number of *distinct rank reductions* found before moving to the new target rank.

def find_lowest_rank_dir():
    for lowest_rank in range(256):
        lowest_rank_dir = f"solutions/{FOLDER_NAME}/{PREFIX}{lowest_rank}"
        if os.path.isdir(lowest_rank_dir):
            return lowest_rank_dir
    return ""

def get_random_file_from_dir(directory):
    if os.path.isdir(directory):
        files = [f for f in Path(directory).iterdir() if f.is_file()]
        return str(random.choice(files)) if files else ""
    return ""


start_overall_time = time.time()


print(f"\n--- Processing target tensor ---")

base_dir = Path("solutions") / f"{FOLDER_NAME}"

print("\n Starting flips")
current_dir = [Path(find_lowest_rank_dir())]
try:
    current_rank = [int(current_dir[0].name[len(PREFIX):])]
except:
    print(f"Couldn't get rank from folder name [{current_dir[0].name}]")
    sys.exit(1)
print(f"--Trying to reduce from rank {current_rank[0]}--")
failed_attempts = [0]
reductions = [0]
stop = [False]
lock = [threading.Lock()]
def flip_worker(thread_id, home_dir):
    while True:
        #choose a file
        with lock[0]:
            input_file = get_random_file_from_dir(base_dir / f"{PREFIX}{current_rank[0]}")
            #do flip
        proc = subprocess.Popen([f"./flip", input_file, PATHLENGTH, "1", "1"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        out, err = proc.communicate()
        out = out.strip()

        #move it to the right place
        try:
            new_file_path_str, new_rank_str = out.split(',', 1)
        except:
            print(f"ERROR: out = {out}")
            return
        new_rank = int(new_rank_str.strip())
        new_file_path = Path(new_file_path_str.strip())

        with lock[0]:
            if new_rank < current_rank[0]:
            #if it is a reduction, add one to the reduction counter
                new_dir = home_dir / f"{PREFIX}{new_rank}"
                new_dir.mkdir(exist_ok=True)
                shutil.move(new_file_path, new_dir / new_file_path.name)

                reductions[0] += 1
                print(f" Reduction found! ({reductions[0]} / {TOTAL_REDUCTIONS_FOUND_THRESHOLD})")
                #if we now have enough reductions, reduce one to the rank counter
                if reductions[0] >= TOTAL_REDUCTIONS_FOUND_THRESHOLD:
                    current_rank[0] -= 1
                    current_dir[0] = Path("solutions") / f"{FOLDER_NAME}" / f"{PREFIX}{current_rank[0]}"
                    reductions[0] = 0
                    failed_attempts[0] = 0
                    print(f"--Trying to reduce from rank {current_rank[0]}--")
            #else stop all workers if too many things
            else:
                new_file_path.unlink()
                failed_attempts[0] += 1
                if reductions[0] == 0 and failed_attempts[0] % 16 == 0:
                    print(f"Failed reductions ({failed_attempts[0]} / {TOTAL_NO_REDUCTION_FLIPS_THRESHOLD})")
                if reductions[0]==0 and failed_attempts[0]>=TOTAL_NO_REDUCTION_FLIPS_THRESHOLD:
                    stop[0] = True
                    #NOTE: IT IS POSSIBLE THAT A REDUCTION IS FOUND IN THE FEW ATTEMPTS AFTER THIS, IN THIS CASE ALL HELL CAN BREAK LOSE. LET'S JUST NOT THINK ABOUT IT TOO HARD
        time.sleep(2) # in case it should stop *soon* (often a problem when they run parallel)
        with lock[0]:
            if stop[0]:
                return
print(f"\n Launching {PARALLEL_INSTANCES} flips on {FOLDER_NAME}\n")
flip_threads = []
for i in range(PARALLEL_INSTANCES):
    t = threading.Thread(target=flip_worker,args=(i,base_dir))
    t.start()
    flip_threads.append(t)

while reductions[0]>0 or failed_attempts[0]<TOTAL_NO_REDUCTION_FLIPS_THRESHOLD:
    time.sleep(1)
for t in flip_threads:
    t.join()


overall_time = time.time() - start_overall_time

print(f"---Finished size after {overall_time // 3600} hrs {(overall_time // 60) % 60} mins {overall_time % 60} secs---")
