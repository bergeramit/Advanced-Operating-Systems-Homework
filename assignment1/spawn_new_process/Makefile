



all:
	gcc ./main.c -o piper

clean:
	rm ./piper

test: all
	@echo "Creating 100 files named: 1, 2, 3.."
	bash -c "touch {1..100}"
	@echo ""
	./piper ls sort head
	@echo ""
	bash -c "rm {1..100}"
	@echo "Should see only 10 files sorted - 1 10 100 11 ..."
