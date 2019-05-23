#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include "Ngram.h"
#include "Vocab.h" 
#include "VocabMap.h"

using namespace std;

const VocabIndex empty_context[] = {Vocab_None};

VocabIndex getIndex(VocabIndex wid, Vocab *voc, Vocab *vocBig5) {
    VocabIndex index = voc->getIndex(vocBig5->getWord(wid));
    if(index == Vocab_None) {
        //OOV
        index = voc->getIndex(Vocab_Unknown);
    }
    return index;
}

int main(int argc, char *argv[]) {
    //disambig -text $file -map $map -lm $language_model -order $order
    string text_filename, map_filename, lm_filename;
    int order;
    int arg_num = 0;
    for (int i = 1; i<argc;i++) {
        string arg = argv[i];
        if (arg == "-text") {
            text_filename = argv[++i];
            arg_num++;
        }
        if (arg == "-map") {
            map_filename = argv[++i];
            arg_num++;
        }
        if (arg == "-lm") {
            lm_filename = argv[++i];
            arg_num++;
        }
        if (arg == "-order") {
            order = atoi(argv[++i]);
            arg_num++;
        }
    }
    if (arg_num != 4) {
        //
    }
    Vocab voc,vocBig5,vocZhuYin;
    Ngram lm( voc, order );
    File lmFile(lm_filename.c_str(), "r" );
    lm.read(lmFile);
    lmFile.close();
    
    VocabMap vmap(vocZhuYin,vocBig5);
    File mapFile(map_filename.c_str(), "r");
    vmap.read(mapFile);
    mapFile.close();

    File textFile(text_filename.c_str(), "r");
    char* line = NULL;

    int  backtrace_pos[250][1200];
    VocabIndex index_pos[250][1200];
    LogP log_proba[250][1200];
    vector<int> candiadtes_num;
    while (line = textFile.getline()){
        // int candiadtes_num[1200];
        candiadtes_num.clear();
        VocabString words[50010];
        words[0] = Vocab_SentStart;//"<s>";
        unsigned int word_length = Vocab::parseWords(line, &(words[1]), 50010);
        words[++word_length] = Vocab_SentEnd;//"</s>";
        // words[word_length++] = Vocab_None;
    	VocabMapIter mapIter(vmap, vocZhuYin.getIndex(words[0]));
        mapIter.init();
        Prob prob;
        VocabIndex w;
        int count = 0;
        while(mapIter.next(w, prob)) {
            log_proba[0][count] = lm.wordProb(getIndex(w,&voc,&vocBig5), empty_context);
            if (log_proba[0][count] == LogP_Zero) {
                log_proba[0][count] = (LogP)(-100.0);
            }
            backtrace_pos[0][count] = -1;
            index_pos[0][count] = w;
            count++;
        }
        candiadtes_num.push_back(count);
        for (int i = 1; i<=word_length;i++) {
            count = 0;
            VocabMapIter mapIter(vmap, vocZhuYin.getIndex(words[i]));
            mapIter.init();
            while (mapIter.next(w, prob)) {
                LogP max_prob = LogP_Zero;
                VocabIndex now_index = getIndex(w,&voc,&vocBig5);
                for (int j=0;j<candiadtes_num.back();j++) {
                    VocabIndex pre_index[] = {getIndex(index_pos[i-1][j], &voc, &vocBig5), Vocab_None};
                    // pow(10,(lm.wordProb( now_index, pre_index)));
                    LogP now_prob = lm.wordProb(now_index, pre_index);
                    LogP pre_prob = lm.wordProb(now_index, empty_context);
                    if (now_prob == LogP_Zero && pre_prob == LogP_Zero) {
                        now_prob = (LogP)(-100.0);
                    }
                    now_prob += log_proba[i-1][j];
                    if (now_prob > max_prob) {
                        // printf("????? %d\n",i);
                        max_prob = now_prob;
                        backtrace_pos[i][count]=j;
                    }
                }
                log_proba[i][count]=max_prob;
                index_pos[i][count]=w;
                count++;
                // printf("count=%d\n",count);
            }
            candiadtes_num.push_back(count);
            // printf("i=%d\n-----\n",i);
        }
        //backtrace path
        LogP max_prob = LogP_Zero;
        int path = 0;
        //get final from maximum prob
        for (int i=0;i<candiadtes_num.back(); i++) {
            if (log_proba[word_length][i] > max_prob) {
                max_prob = log_proba[word_length][i];
                path = i;
            }
        }
        vector<string> output;
        //need to ignore </s>
        for (int i = word_length; i>0;i--) {
            output.push_back(vocBig5.getWord(index_pos[i][path]));
            // printf("%s\n",output[i]);
            path = backtrace_pos[i][path];
        }
        cout << "<s>";
        reverse(output.begin(), output.end());
        for (vector<string>::iterator it = output.begin() ; it != output.end(); it++) {
            cout << ' ' << *it;
        }
        cout << endl;
    }
    return 0;
}