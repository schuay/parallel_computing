R = R
LATEX = pdflatex

RFILE = report.Rnw
LFILE = $(RFILE:.Rnw=.tex)

bake:
	$(R) CMD Sweave $(RFILE)
	$(LATEX) $(LFILE)
	$(LATEX) $(LFILE)

clean:
	rm -rf *.tex *.aux *.log *.pdf *.toc *.eps
