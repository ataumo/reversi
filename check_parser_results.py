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
    fcible=open('fail-soft.txt','a')
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

def test(name,run,enter,expect):
    print('\n')
    print("test",name,': ', end='')
    result = subprocess.Popen(run,stdin = subprocess.PIPE, encoding = 'utf8',stdout=subprocess.DEVNULL)
    result.stdin.write(enter)
    result.stdin.close()
    result.wait()
    if expect == 'success':
        if (result.returncode == 0):
            print("pass-------------------------------------------------")
        else:
            print("fail------------------------------------------------X")
    else:
        if (result.returncode == 0):
            print("fail------------------------------------------------X")
        else :
            print("pass-------------------------------------------------")

def test_serie():
    test(1,["./reversi"],'Q\n\n','success')
    test(2,["./reversi",'-x'],'Q\n\n','')
    test(3,["./reversi",'-v'],'Q\n\n','success')
    test(4,["./reversi",'--verbose'],'Q\n\n','success')
    test(5,["./reversi",'-V'],'Q\n\n','success')
    test(6,["./reversi",'--version'],'Q\n\n','success')
    test(7,["./reversi",'-h'],'Q\n\n','success')
    test(8,["./reversi",'--help'],'Q\n\n','success')
    test(9,["./reversi",'--he'],'Q\n\n','success')

    test(10,["./reversi",'-b'],'Q\n\n','success')
    test(11,["./reversi",'--black-ai'],'Q\n\n','success')
    test(12,["./reversi",'-b0'],'Q\n\n','success')
    test(13,["./reversi",'--black-ai=0'],'Q\n\n','success')
    test(14,["./reversi",'-b1'],'Q\n\n','success')
    test(15,["./reversi",'--black-ai=1'],'Q\n\n','success')

    test(16,["./reversi",'-w'],'Q\n\n','success')
    test(17,["./reversi",'--white-ai'],'Q\n\n','success')
    test(18,["./reversi",'-w0'],'Q\n\n','success')
    test(19,["./reversi",'--white-ai=0'],'Q\n\n','success')
    test(20,["./reversi",'-w1'],'Q\n\n','success')
    test(21,["./reversi",'--white-ai=1'],'Q\n\n','success')

    test(22,["./reversi",'-b','-w'],'Q\n\n','success')
    test(23,["./reversi",'--black-ai','--white-ai'],'Q\n\n','success')
    test(24,["./reversi",'-b0','-w1'],'Q\n\n','success')
    test(25,["./reversi",'--black-ai=0','--white-ai=1'],'Q\n\n','success')

    test(26,["./reversi",'-s'],'Q\n\n','')
    test(27,["./reversi",'--size'],'Q\n\n','')
    test(28,["./reversi",'-s','1'],'Q\n\n','success')
    test(29,["./reversi",'--size','1'],'Q\n\n','success')
    test(30,["./reversi",'-s','2'],'Q\n\n','success')
    test(31,["./reversi",'--size','2'],'Q\n\n','success')
    test(32,["./reversi",'-s','3'],'Q\n\n','success')
    test(33,["./reversi",'--size','3'],'Q\n\n','success')
    test(34,["./reversi",'-s','4'],'Q\n\n','success')
    test(35,["./reversi",'--size','4'],'Q\n\n','success')
    test(36,["./reversi",'-s','5'],'Q\n\n','success')
    test(37,["./reversi",'--size','5'],'Q\n\n','success')
    test(38,["./reversi",'-s','0'],'Q\n\n','')
    test(39,["./reversi",'--size','0'],'Q\n\n','')
    test(38,["./reversi",'-s','6'],'Q\n\n','')
    test(39,["./reversi",'--size','6'],'Q\n\n','')

    #argument parser
    test(40,["./reversi",'board.txt'],'Q\n\n','success')
    test(41,["./reversi",'-v','board.txt'],'Q\n\n','success')
    test(42,["./reversi",'--verbose','board.txt'],'Q\n\n','success')
    test(43,["./reversi",'-c'],'Q\n\n','')
    test(44,["./reversi",'--contest'],'Q\n\n','')
    test(45,["./reversi",'-c','missing.txt'],'Q\n\n','')
    test(46,["./reversi",'--contest','missing.txt'],'Q\n\n','')
    test(47,["./reversi",'-c','shadow'],'Q\n\n','')
    test(48,["./reversi",'--contest','shadow'],'Q\n\n','')
    test(49,["./reversi",'-c','board.txt'],'Q\n\n','success')
    test(50,["./reversi",'--contest','board.txt'],'Q\n\n','success')
    test(51,["./reversi",'-c','-v','board.txt'],'Q\n\n','success')
    test(52,["./reversi",'--contest','--verbose','board.txt'],'Q\n\n','success')

    #file parser
    i=52
    for f in listdir("board_parser/"):
        i=i+1
        filename = "board_parser/"+f
        print('\n'+f)
        if ("pass" in f):
            test(i,["./reversi",filename],'Q\n\n','success')
        else:
            test(i,["./reversi",filename],'Q\n\n','')




#test_serie()

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
        process = subprocess.Popen(["./reversi","-s4","-b4","-w1"], stdout=subprocess.PIPE)
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
