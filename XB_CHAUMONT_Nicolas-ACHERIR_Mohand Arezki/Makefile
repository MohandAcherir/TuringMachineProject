test: TM_launch.c
	# Partie I
	gcc -o launch TM_launch.c
	./launch launch_desc1.txt
	./launch launch_desc2.txt
	./launch launch_desc3.txt
	
	# Partie II
	gcc -o TM_21 TM_21.c
	./TM_21 desc_21.txt
	./launch Result_21.txt

	gcc -o TM_22 TM_22.c
	./TM_22 desc_22.txt
	./launch Result_22.txt

	#Partie III
	gcc -o TM_31 TM_31.c
	./TM_31 desc_31.txt

	gcc -o TM_32 TM_32.c
	./TM_32 desc_32.txt

clean: 
	rm -f launch TM_21 TM_22 TM_31 TM_32
