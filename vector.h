#ifndef VECTOR_H
#define VECTOR_H

// Minimal class to replace std::vector
template<typename Data>
class Vector {
		size_t d_size; // Stores no. of actually stored objects
		size_t d_capacity; // Stores allocated capacity
		Data d_data[10]; // Stores data
	public:
		Vector() :
			d_size(0),
			d_capacity(10)
			//d_data( 0 )
		{}; // Default constructor
		Vector(Vector const &other) :
			d_size(other.d_size),
			d_capacity(other.d_capacity),
			d_data(0) {
			d_data = (Data *)malloc(d_capacity * sizeof(Data));
			memcpy(d_data, other.d_data, d_size * sizeof(Data));

		}; // Copy constuctor

		~Vector() {
			free(d_data);

		}; // Destructor

		Vector &operator=(Vector const &other) {
			free(d_data);
			d_size = other.d_size;
			d_capacity = other.d_capacity;
			d_data = (Data *)malloc(d_capacity * sizeof(Data));
			memcpy(d_data, other.d_data, d_size * sizeof(Data));
			return *this;

		}; // Needed for memory management

		void push_back(Data const &x) {
			//if ( d_capacity == d_size )
			//resize();
			d_data[d_size++] = x;
		}; // Adds new value. If needed, allocates more space

		void remove(int idx) {
			if(idx >= d_size) return;
			if(idx - 1 != d_size) {
				for(size_t at = idx + 1; at < d_size; ++at) {
					d_data[at-1] = d_data[at];
				}
			}
			d_size--;
		}; // Adds new value. If needed, allocates more space

		size_t size() const {
			return d_size;

		}; // Size getter

		Data const &operator[](size_t idx) const {
			return d_data[idx];

		}; // Const getter

		Data &operator[](size_t idx) {
			return d_data[idx];

		}; // Changeable getter
	private:
		void resize() {
			d_capacity = d_capacity ? d_capacity * 2 : 1;
			Data *newdata = (Data *)malloc(d_capacity * sizeof(Data));
			memcpy(newdata, d_data, d_size * sizeof(Data));
			free(d_data);
			d_data = newdata;
		};// Allocates double the old space
};
#endif
