#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Damping factor
double d = 0.85;
long num_vertices = -1;
double delta = 0;

// Struct to hold each vertex
typedef struct {
	double r_init;
	double r_final;
	long num_edges;
	int counter;
} Vertex;

/*
 * Updates the Vertex to the current iteration
 * @v - vertex to update
 * @counter - current iteration counter
 */
void updateCounter(Vertex *v, int counter) {
	if(v->counter == counter)
		return;

	v->counter = counter;
	double new_init = ((1-d)) + (d*v->r_final);
	if(fabs(new_init - v->r_init) > delta)
		delta = fabs(new_init - v->r_init);

	v->r_init = new_init;
	v->r_final = 0;
}

/*
 * Splits a line into two longs
 * @line - the string to parse
 * @num1 - where the first long will be placed
 * @num2 - where the second long will be placed
 */
void parseLine(char *line, long *num1, long *num2) {
	char *end_ptr;
	*num1 = strtol(line, &end_ptr, 10);
	*num2 = strtol(end_ptr + 1, &end_ptr, 10);
}

/*
 * Initializes a vertex position
 * @v - pointer to array of vertices
 * @p - position of the vertex
 */
void initVertex(Vertex *v, long p) {
	if(v[p].r_init != 5) {
		v[p].r_init = 5;
		v[p].r_final = 0;
		v[p].num_edges = 0;
		v[p].counter = 0;
	}
}

int main(int argc, char **argv) {

	if(argc != 2) {
		if(argc == 1)
			printf("%s\n", "Too few arguments.");
		else
			printf("%s\n", "Too many arguments.");
		return -1;
	}

	FILE * f_in = fopen(argv[1], "r");
	if(f_in == NULL) {
		perror("Error: ");
		return -1;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	long pred; //predecessor
	long succ; //successor

	int v_size = 16;
	Vertex *vertices = (Vertex *) calloc(v_size, sizeof(Vertex));

	while((read = getline(&line, &len, f_in)) != -1) {
		parseLine(line, &pred, &succ);
		if(pred > num_vertices)
			num_vertices = pred;
		
		if(succ > num_vertices)
			num_vertices = succ;

		while(num_vertices > v_size-1) {
			v_size = v_size*2;
			if(!(vertices = (Vertex *) realloc(vertices, v_size*sizeof(Vertex)))) {
				puts("Out of memory for the graph vertices.");
				return -1;
			}
			memset((void *)vertices + (sizeof(Vertex)*(v_size/2)), 0, (v_size/2)*sizeof(Vertex));
		}
		
		initVertex(vertices, pred);
		initVertex(vertices, succ);
		vertices[pred].num_edges++;

	}
	num_vertices++;

	for(int i = 0; i < 100; i++) {
		if(i > 1 && delta < .0001) {
			//printf("i:%d | delta:%lf\n", i, delta);
			break;
		}
		delta = 0;

		rewind(f_in);

		while((read = getline(&line, &len, f_in)) != -1) {
			parseLine(line, &pred, &succ);
			Vertex *p = &vertices[pred];
			Vertex *s = &vertices[succ];

			updateCounter(p, i);
			updateCounter(s, i);
			
			s->r_final += p->r_init / p->num_edges;
		}
	}

	fclose(f_in);
	free(line);

	double total = 0;
	for(int i = 0; i < num_vertices; i++)
		total += vertices[i].r_init;

	for(int i = 0; i < num_vertices; i++)
		printf("%d = %lf\n", i, vertices[i].r_init/total);

	free(vertices);

	return 0;
}
