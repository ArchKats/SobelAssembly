SRCDIR:=src/

all: 

%:
	@make --no-print-directory -C $(SRCDIR) $@
