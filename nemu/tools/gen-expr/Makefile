APP=gen-expr
APP_DEBUG=gen-expr-debug

$(APP): gen-expr.c
	gcc -O2 -Wall -Werror -o $@ $<

$(APP_DEBUG): gen-expr.c
	gcc -O2 -Wall -Werror -g -o $@ $<

.PHONY: clean
clean:
	-rm $(APP) $(APP_DEBUG) 2> /dev/null
