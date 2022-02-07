#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import glob
import difflib
from subprocess import Popen, PIPE
from threading import Timer
from util import *
from itertools import zip_longest

MAX_TEST_RUN_TIME = 5
BONUS_POINTS = 20
def timeoutProcess(process, verbose = False):
    if verbose:
        print("Test timed out. Killing...")
    process.kill()

def runTest(program, inFile, outFile, verbose = False):
    if isinstance(program, str):
        program = program.split()
    exit_code, err, output = run_program(program, inFile, verbose=verbose, timeout=MAX_TEST_RUN_TIME)
    if exit_code == 0:
        with open(outFile, "r") as f:
            expected = [l for l in f.readlines()]
        # Trim new lines at the end
        while len(expected) > 0 and expected[len(expected) - 1] == "\n":
            del expected[len(expected) - 1]
        while len(output) > 0 and output[len(output) - 1] == "\n":
            del output[len(output) - 1]
        success = True
        for gold, pred in zip_longest(expected, output):
            if gold is None or pred is None:
                success = False
                break
            partsGold = gold.split(" ")
            partsPred = pred.split(" ")
            for i in range(max(len(partsGold), len(partsPred))):
                if i == 0:
                    goldSteps = int(partsGold[i])
                    predSteps = int(partsPred[i])
                    if predSteps < goldSteps * 0.8 or predSteps > goldSteps * 1.2:
                        success = False
                        break
                elif i > len(partsGold) or i > len(partsPred):
                    success = False
                    break
                else:
                    if partsGold[i] != partsPred[i]:
                        success = False
                        break

        diffs = list(difflib.ndiff(expected, output))
        failedDiffs = [f"\t{i + 1:5d}|{l}" for i, l in enumerate(diffs) if l[0] in {"+", "-", "?"}]
        if not success:
            if verbose:
                print("Test produced incorrect output:")
                print("".join(failedDiffs[:30]))
                if len(failedDiffs) > 30:
                    print(">> Diff truncated as too long.\n")
                
            return False
        else:
            if verbose:
                print("Test passed")
            return True
    else:
        if verbose:
            if exit_code != 0:
                print(f"Program exited with code: {exit_code} ({get_code_reason(exit_code)})")
            if err is not None:
                print("Error occured during execution:", err)
        return False


def testProgram(program, tests, maxPoints, verbose = False):
    if verbose > 1:
        print(f"Testing `{program}` with tests from: `{tests}`")
    if not os.path.isfile(program):
        if verbose > 0:
            print(f"Program `{program}` does not exist")
        return 0
    if not os.path.isdir(tests):
        if verbose > 0:
            print(f"Tests directory: `{tests}` does not exist")
        return None
    totalTests = 0
    testsPassed = 0
    totalBonusTests = 0
    bonusTestsPassed = 0
    for inFile in glob.glob(f"{tests}/test_*.in"):
        # Find the corresponding out file
        testName = inFile.split("/")[-1][5:-3]
        
        outFile = inFile[:-2] + "out"
        if os.path.isfile(outFile):
            if verbose > 1:
                print(f"Running test: {testName}")
            
            if "astar" in inFile:
                totalBonusTests += 1
                result = runTest(program + " astar", inFile, outFile, verbose = verbose > 1)
                if result:
                    bonusTestsPassed += 1
            else:
                totalTests += 1
                result = runTest(program + " bfs", inFile, outFile, verbose = verbose > 1)
                if result:
                    testsPassed += 1
            if verbose == 1:
                if result:
                    print("+", end="", flush=True)
                else:
                    print("-", end="", flush=True)
    if verbose == 1:
        print("\n", end="")
    pointsAchieved = round(maxPoints * (testsPassed / totalTests)) + round(BONUS_POINTS * (bonusTestsPassed / totalBonusTests)) if totalTests > 0 and totalBonusTests > 0 else "ERR_UNDEFINED_TESTS"
    if verbose > 0:
        print(f"Passed {testsPassed} out of {totalTests} tests and {bonusTestsPassed} out of {totalBonusTests} bonus tests, resulting in {pointsAchieved} points.")
    return pointsAchieved
                
    

if __name__ == '__main__':
    prog = sys.argv[1]
    testFolder = sys.argv[2]
    maxPoints = int(sys.argv[3])
    verbose = int(sys.argv[4]) if len(sys.argv) > 4 else 0
    finalScore = testProgram(prog, testFolder, maxPoints, verbose=verbose)
    print(finalScore)
