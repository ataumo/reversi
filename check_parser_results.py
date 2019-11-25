from os import listdir
import subprocess
import time

EXIT_SUCCESS = 0

def stock_data(l,depth):
    L=[]
    char=''
    for i in range(len(l)):
        #if l[i]!=0:
        char=str(depth)+' '+str(l[i])
        L+=[char]
    fcible=open('alphabeta.txt','a')
    print("ecriture dans le fichier..")
    fcible.write('\n'.join(L))
    fcible.close()

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

def test_game_time(n,depth):
    ticks = time.time()
    L=[]
    nbr_win = 0
    nbr_win_o = 0
    nbr_draw = 0
    nbr_time_echec = 0
    nbr_test = n
    p=0
    for i in range(nbr_test):
        current_time = time.time()
        process = subprocess.Popen(["./reversi","-s4","-b5","-w1"], stdout=subprocess.PIPE)
        stdout = process.communicate()[0]
        if "Player 'X' win the game." in 'STDOUT:{}'.format(stdout):
            nbr_win=nbr_win+1
        elif "Player 'O' win the game." in 'STDOUT:{}'.format(stdout):
            nbr_win_o=nbr_win_o+1
        elif "Draw" in 'STDOUT:{}'.format(stdout):
            nbr_draw=nbr_draw+1
        elif "echec time" in 'STDOUT:{}'.format(stdout):
            nbr_time_echec=nbr_time_echec+1
        np=(i*80)//nbr_test #print 80 character
        if np>p:
            print("%", sep=' ', end='', flush=True)
            p=np
        #L.append(time.time()-current_time)
    #stock_data(L,depth)
    print(' ')
    print(nbr_test,"tests ")
    print(nbr_win*100/nbr_test,"% of success for X")
    print(nbr_win_o*100/nbr_test,"% of success for O")
    print(nbr_draw*100/nbr_test, "% of draw game")
    print(nbr_time_echec, " of time echec")
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
test_game_time(1000,5)
