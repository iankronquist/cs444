CC=gcc
CFLAGS=-Werror -Wall -Wextra -std=c99 -g -lpthread

# Latex stuff
LATEX	= latex -shell-escape
BIBTEX	= bibtex
DVIPS	= dvips
DVIPDF  = dvipdft
XDVI	= xdvi -gamma 4
GH		= gv

EXAMPLES = $(wildcard *.h)
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
TRG	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)


all: producer-consumer pdf

pdf: $(PDF)

ps: $(PSF)

$(TRG): %.dvi: %.tex *.bib $(EXAMPLES)
	$(LATEX) $<
	$(BIBTEX) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

$(PSF):%.ps: %.dvi
	$(DVIPS) -R -Poutline -t letter $< -o $@

$(PDF): %.pdf: %.ps
#	$(DVIPDF) -o $@ $<
	ps2pdf $<

show: $(TRG)
	@for i in $(TRG) ; do $(XDVI) $$i & done

showps: $(PSF)
	@for i in $(PSF) ; do $(GH) $$i & done



debug: CFLAGS += -g -D DEBUG -D BUFFER_SIZE=4
debug: producer-consumer

producer-consumer: producer-consumer.c ass1_rand.c ass1_rand_mersenne.c ass1_rand.S
	$(CC) producer-consumer.c ass1_rand.c ass1_rand_mersenne.c ass1_rand.S -o producer-consumer $(CFLAGS)

clean:
	rm -f *.o producer-consumer
	rm -rf *.dSYM/
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg


.PHONY: all show clean ps pdf showps debug
