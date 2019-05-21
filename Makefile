# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH ?= /home/master/07/r07922009/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

TESTDIR=result1
TESTDATA=testdata

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

build_lm:
	perl separator_big5.pl corpus.txt > corpus_seg.txt
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -text corpus_seg.txt -write lm.cnt -order 2
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

test:
	[ -d $(TESTDIR) ] || mkdir -p $(TESTDIR);
	@for i in $(shell seq 1 10); do \
		$(SRIPATH)/bin/$(MACHINE_TYPE)/disambig -text $(TESTDATA)/$${i}.txt -map $(TO) -lm $(LM) -order 2 > $(TESTDIR)/$${i}.txt; \
	done;

run:
	@#TODO How to run your code toward different txt? 
	@for i in $(shell seq 1 10) ; do \
	    echo "Running $$i.txt"; \
	    ./mydisambig -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > result2/$$i.txt; \
	done;
map:
	@#TODO How to map?
	@echo "Mapping!"
	@#./mapping $(FROM) $(TO)
	python mapping.py $(FROM) $(TO)
	@#sh mapping.sh $(FROM) $(TO)
	@#perl mapping.pl Big5-ZhuYin.map ZhuYin-Big5.map
clean:
	$(RM) $(OBJ) $(TARGET)

