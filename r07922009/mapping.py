#!/usr/bin/python3
import sys

def main():

    zhuyin_to_character = {}
    # 一 ㄧˊ/ㄧˋ/ㄧˍ
    with open(sys.argv[1], 'r', encoding='big5-hkscs') as inputfile:
        for line in inputfile:
            character, zhuyin_items = line.split(' ')
            zhuyins = zhuyin_items.split('/')

            for zhuyin in zhuyins:
                if zhuyin[0] in zhuyin_to_character:
                    zhuyin_to_character[zhuyin[0]].append(character)
                else:
                    zhuyin_to_character[zhuyin[0]] = [character]
        
            zhuyin_to_character[character] = [character]
            
    with open(sys.argv[2], 'w', encoding='big5-hkscs') as outputfile:
        for key, value in zhuyin_to_character.items():
        #注意：第一個字/注音後面接的是tab，且後面每一個字之間都有空格。
            outputfile.write(key + '\t' +  ' '.join(value) + '\n')

if __name__ == '__main__':
    main()