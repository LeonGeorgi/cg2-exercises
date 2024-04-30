#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_primitive.h"


template <typename T>
struct sphere : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	sphere() { implicit_base<T>::gui_color = 0xFF8888; }
	std::string get_type_name() const { return "sphere"; }

	/// Evaluate the sphere quadric at p
	T evaluate(const pnt_type& p) const
	{
		return p.sqr_length() - 1.0;
	}

	/// Evaluate the gradient of the sphere quadric at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		return p / p.sqr_length();
	}

	void create_gui()
	{
		implicit_primitive<T>::create_gui();
	}
};

scene_factory_registration<sphere<double> > sfr_sphere("sphere;S");
