a.out: edge_detector.c
	gcc -Wall -g -o edge_detector edge_detector.c

test: a.out
	@for file in tests/sample_photos/*; do \
		./edge_detector $$file > /dev/null && \
		if [ -f tests/sample_filtered_photos/`basename $$file .ppm`_laplacian.ppm ]; then \
			if diff laplacian1.ppm tests/sample_filtered_photos/`basename $$file .ppm`_laplacian.ppm; then \
				echo "Files are the same: $$file"; \
			else \
				echo "Files are different: $$file"; \
				exit 1; \
			fi; \
		fi; \
	done

clean:
	rm -f edge_detector
	rm -f laplacian*.ppm
