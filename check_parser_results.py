from os import listdir
import subprocess
import time

EXIT_SUCCESS = 0

def test_file_parser():
    for f in listdir("board_parser/"):
        filename = "board_parser/"+f
        print('\n'+f)
        result = subprocess.run(["./reversi", filename])#,
                                #stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if (("pass" in f and result.returncode == 0) or ("fail" in f and result.returncode != 0)):
            pass
        else:
            print("error on file "+f)
            exit(-1)
    print("sucess !!")

def test_game_time(n):
    ticks = time.time()
    nbr_win = 0
    nbr_test = n
    p=0
    for i in range(nbr_test):
        process = subprocess.Popen(["./reversi","-s4","-b3","-w1"], stdout=subprocess.PIPE)
        stdout = process.communicate()[0]
        if "Player 'X' win the game." in 'STDOUT:{}'.format(stdout):
            nbr_win=nbr_win+1
        np=(i*100)//nbr_test
        if np>p:
            print("%", sep=' ', end='', flush=True)
            p=np
    print(' ')
    print(nbr_test,"tests ",nbr_win*100/nbr_test,"% of success for X")
    print(time.time()-ticks,"seconde")

def check_test():
    print("test 1:")
    result = subprocess.Popen(["./reversi", "-c", "missing.txt"],stdin = subprocess.PIPE, encoding = 'utf8',stdout=subprocess.DEVNULL)
    result.stdin.write('Q\n\n')
    result.stdin.close()
    result.wait()
    if result.returncode==EXIT_SUCCESS:
        print("EXIT_SUCCESS")
    else:
        print(result.returncode)

    print("test 1:")
    result = subprocess.Popen(["./reversi", "-s0"],stdin = subprocess.PIPE, encoding = 'utf8',stdout=subprocess.DEVNULL)
    result.stdin.write('Q\n\n')
    result.stdin.close()
    result.wait()
    if result.returncode==EXIT_SUCCESS:
        print("EXIT_SUCCESS")
    else:
        print(result.returncode)

#check_test()
test_game_time(1000)
