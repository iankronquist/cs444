obj-m := ebd.o
KDIR := ../linux-yocto-3.14
PWD := $(shell pwd)

LATEXINSTALL=/usr/local/apps/tex_live/current/bin/x86_64-linux/
# Latex stuff
LATEX	= $(LATEXINSTALL)latex -shell-escape
BIBTEX	= $(LATEXINSTALL)bibtex
DVIPS	= $(LATEXINSTALL)dvips
DVIPDF  = $(LATEXINSTALL)dvipdft
XDVI	= $(LATEXINSTALL)xdvi -gamma 4
GH		= $(LATEXINSTALL)gv

EXAMPLES = $(wildcard *.h)
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
TRG	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)


all: pdf
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

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


clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *-blx.bib *.run.xml
	rm -f *.o *.ko *.ko.unsigned modules.order Module.symvers ebd.mod.c .*.cmd
	rm -rf .tmp_versions

.PHONY: all show clean ps pdf showps debug test
