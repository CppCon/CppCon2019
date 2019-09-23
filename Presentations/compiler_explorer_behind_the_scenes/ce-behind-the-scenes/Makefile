default: all

all: images/ce_aws.svg

images/ce_aws.svg: ce.dot
	dot -Tsvg -o$@ $^
