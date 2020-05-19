#pragma once
#include "vector3d.hpp"

typedef float vec_t;

class view_matrix_t {
public:

	view_matrix_t();
	view_matrix_t(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	view_matrix_t(const vec3_t& forward, const vec3_t& left, const vec3_t& up);

	// Set the values in the matrix.
	void init(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);

	// array access
	inline float* operator[](int i) {
		return m[i];
	}

	inline const float* operator[](int i) const {
		return m[i];
	}

	// Get a pointer to m[0][0]
	inline float* base() {
		return &m[0][0];
	}

	inline const float* base() const {
		return &m[0][0];
	}

	void		set_left(const vec3_t& vLeft);
	void		set_up(const vec3_t& vUp);
	void		set_forward(const vec3_t& vForward);

	void		set_basis_vector_3d(const vec3_t& vForward, const vec3_t& vLeft, const vec3_t& vUp);

	// Get/set the translation.
	vec3_t& get_translation(vec3_t& vTrans) const;
	void		set_translation(const vec3_t& vTrans);

	void		pre_translate(const vec3_t& vTrans);
	void		post_translate(const vec3_t& vTrans);

	matrix_t& as_matrix();
	const matrix_t& as_matrix() const;

	bool		operator!=(const view_matrix_t& src) const { return *this != src; }

	// Access the basis vec3_ts.
	vec3_t		get_left() const;
	vec3_t		get_up() const;
	vec3_t		get_forward() const;
	vec3_t		get_translation() const;

	// Matrix->vec3_t operations.
public:
	// Multiply by a 3D vec3_t (same as operator*).
	void		vector_3d_multiply(const vec3_t & vIn, vec3_t & vOut) const;

	// Multiply by a 4D vec3_t.
	//void		V4Mul( const vec3_t4D &vIn, vec3_t4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	vec3_t		apply_rotation(const vec3_t & vVec) const;

	// Multiply by a vec3_t (divides by w, assumes input w is 1).
	vec3_t		operator*(const vec3_t & vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	vec3_t		vector_3d_multiply(const vec3_t & vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	vec3_t		vector_3d_transpose_rotation(const vec3_t & vVec) const;

	// Multiply by the upper 3 rows.
	vec3_t		vector_3d_multiply_upper(const vec3_t & vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	vec3_t		vector_3d_transpose(const vec3_t & vVec) const;

	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane		operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	view_matrix_t& operator=(const view_matrix_t & mOther);

	// Add two matrices.
	const view_matrix_t& operator+=(const view_matrix_t & other);

	// Add/Subtract two matrices.
	view_matrix_t		operator+(const view_matrix_t & other) const;
	view_matrix_t		operator-(const view_matrix_t & other) const;

	// Negation.
	view_matrix_t		operator-() const;

	// Matrix operations.
public:
	// Set to identity.
	void		identity();

	bool		is_identity() const;
public:
	// The matrix.
	vec_t		m[4][4];
};