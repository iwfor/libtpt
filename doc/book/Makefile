XMLSHARE=	/opt/local/share/xsl/docbook-xsl

XSLT=		xsltproc

XSLTFLAGS=	--timing

SRC=		main.xml

default:	html

all: html-single html-split

html-single: Makefile $(SRC)
	$(XSLT) $(XSLTFLAGS) -o libtpt-book.html $(XMLSHARE)/html/docbook.xsl $(SRC)

html-split: Makefile $(SRC) 
	mkdir -p html
	$(XSLT) $(XSLTFLAGS) -o html/ $(XMLSHARE)/html/chunk.xsl $(SRC)

clean:
	rm -f html/*.html *.html core
