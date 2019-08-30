import argparse
import sys

def main():
    file_name = sys.argv[1]
    array = read_csv(file_name)
    x_num = count_x(array)
    y_num = len(array) / x_num
    y_num = int(y_num)
    gen_new_xfile(x_num, y_num,  array)
    gen_new_yfile(x_num, y_num, array)

    return

def read_csv(file_name):
    with open(file_name, mode='r', encoding='utf-8') as f:
        inarray = []
        for row in f:
            colomuns = row.rstrip().split(',')
            a = float(colomuns[0])
            b = float(colomuns[1])
            c = float(colomuns[2])
            in_array = [a, b, c]
            inarray.append(in_array)
        return inarray

def count_x(array):
    counter = 0
    pre_element = array[counter][0]
    while(array[counter][0] == pre_element):
        pre_element = array[counter][0]
        counter += 1
    return counter

def gen_new_xfile(x_num, y_num, array):
    for i in range(y_num):
        with open('x_'+str(i)+'X='+str(array[i*x_num][0])+'.csv', mode='w', encoding='utf-8') as f:
            for j in range(x_num):
                y_key = array[j+i*x_num][1]
                y_value = array[j+i*x_num][2]
                row = str(y_key) + ',' + str(y_value) + '\n'
                f.writelines(row)


def gen_new_yfile(x_num, y_num, array):
    for i in range(x_num):
        with open('y_'+str(i)+'Y='+str(array[i][1])+'.csv', mode='w', encoding='utf-8') as f:
            for j in range(y_num):
                x_key = array[j*x_num+i][0]
                x_value = array[j*x_num+i][2]
                row = str(x_key) + ',' + str(x_value) + '\n'
                f.writelines(row)

if __name__ == '__main__':
    main()
