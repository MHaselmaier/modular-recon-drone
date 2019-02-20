ADOC=asciidoctor -r asciidoctor-diagram
ADOC_PDF=$(ADOC) -r asciidoctor-pdf -b pdf

all: pdf html

pdf: documentation.adoc
	$(ADOC_PDF) $<

html: documentation.adoc
	$(ADOC) $<

clean:
	rm -f *.html *.pdf