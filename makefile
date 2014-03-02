SRC		= src
TEST_SRC	= test
BUILD		= build
BUILD_TEST	= $(BUILD)/test
BUILD_NUMBER	?= dev

ifndef HOMEBREW_ROOT
PREFIX?=/usr/local
else
PREFIX?=$(HOMEBREW_ROOT)
endif

LIB_PATH = $(PREFIX)/lib
LT = a

LIBS		= \
			$(LIB_PATH)/libboost_filesystem-mt.$(LT) 	\
			$(LIB_PATH)/libboost_system-mt.$(LT) 		\
			$(LIB_PATH)/libboost_program_options-mt.$(LT)	\
			$(LIB_PATH)/libboost_regex-mt.$(LT)		\
			$(LIB_PATH)/libboost_thread-mt.$(LT)

TEST_LIBS	= 	$(LIB_PATH)/libboost_unit_test_framework-mt.$(LT)

OBJECTS 	=	$(BUILD)/build_daemon.o


TEST_OBJECTS	=	$(BUILD_TEST)/test_main.o	\
			$(BUILD_TEST)/hidden_file_filter_test.o	\
			$(BUILD_TEST)/ignore_reader_tests.o	\
			$(BUILD_TEST)/file_pattern_parser_test.o

all		:	build/lazybuilder test

$(BUILD)	:
	-mkdir $@

$(BUILD_TEST)		:	$(BUILD)
	-mkdir $@

$(BUILD)/lazybuilder	:	$(OBJECTS) $(BUILD)/main.o
	clang++ -g -O1 -o $@ -std=c++11 -D BUILD_NUMBER='"$(BUILD_NUMBER)"' -Xclang "-stdlib=libc++" -lc++ $^ -I $(PREFIX)/include -framework CoreFoundation -framework CoreServices $(LIBS)

$(BUILD)/lazybuilder-test	:	$(OBJECTS) $(TEST_OBJECTS)
	clang++ $^ -o $@ -std=c++11 -lc++ $(LIBS) $(TEST_LIBS)  -framework CoreFoundation -framework CoreServices

test			:	$(BUILD_TEST) $(BUILD)/lazybuilder-test
	./$(BUILD)/lazybuilder-test

ci-test: results.xml

results.xml	: $(BUILD)/lazybuilder-test
	./$^ --log_format=XML --log_sink=results.xml --log_level=all --report_level=no

ci-build:	ci-test dmg

dist	:
	-mkdir dist

dmg:	lazybuilder-install-0.0.$(BUILD_NUMBER).dmg

lazybuilder-install-0.0.$(BUILD_NUMBER).dmg : $(BUILD)/lazybuilder dist/README.html
	ln -s $(PREFIX)/bin dist/bin
	cp $(BUILD)/lazybuilder dist
	hdiutil create tmp.dmg -ov -volname "lazybuilder - duplicate finder" -fs HFS+ -srcfolder "dist" 
	hdiutil convert tmp.dmg -format UDZO -o lazybuilder-install-0.0.$(BUILD_NUMBER).dmg
	-rm tmp.dmg

dist/README.html	:	dist	README.md
	markdown README.md > dist/README.html

install:	$(BUILD)/lazybuilder
	cp $(BUILD)/lazybuilder $(PREFIX)/bin

clean:
	-rm -rf $(BUILD)/*
	-rm build-daemen-install*.dmg
	-rm tmp.dmg
	-rm -rf dist

$(BUILD)/%.o : $(SRC)/%.cpp	$(BUILD)
	clang++ -g -O1 -std=c++11 -Xclang "-stdlib=libc++" -I $(SRC) -I $(PREFIX)/include -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.c	$(BUILD)
	clang -g -O1 -I $(SRC) -I $(PREFIX)/include -c $< -o $@

$(BUILD_TEST)/%.o : $(TEST_SRC)/%.cpp	$(BUILD_TEST)
	clang++ -g -O1 -std=c++11 -Xclang "-stdlib=libc++" -I $(SRC) -I $(PREFIX)/include -D MAKEFILE_BUILD -c $< -o $@


