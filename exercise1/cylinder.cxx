#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_primitive.h"


template <typename T>
struct cylinder : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	cylinder() { implicit_base<T>::gui_color = 0xFF8888; }
	std::string get_type_name() const { return "cylinder"; }

	/// Evaluate the implicit cylinder function at p
	T evaluate(const pnt_type& p) const
	{
		return p[0] * p[0] + p[1] * p[1] - 1.0;
	}

	/// Evaluate the gradient of the implicit cylinder function at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		float distance = std::sqrt(p[0] * p[0] + p[1] * p[1]);
		return vec_type(p[0] / distance, p[1] / distance, 0);
	}

	void create_gui()
	{
		implicit_primitive<T>::create_gui();
	}
};

scene_factory_registration<cylinder<double> > sfr_cylinder("cylinder;Y");
