#include <cgv/math/fvec.h>
#include "implicit_primitive.h"


template <typename T>
struct box : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	box() {}
	std::string get_type_name() const { return "box"; }
	void on_set(void* member_ptr) { implicit_base<T>::update_scene(); }

	/*********************************************************************************/
	/* Task 1.1a: If you need any auxiliary functions for this task, put them here.  */

	// < your code >

	/* [END] Task 1.1a
	/*********************************************************************************/

	/// Evaluate the implicit box function at p
	T evaluate(const pnt_type& p) const
	{
		return std::max(abs(p[0]), std::max(abs(p[1]), abs(p[2]))) - 1.0;
	}

	/// Evaluate the gradient of the implicit box function at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		if (abs(p[0]) > abs(p[1]) && abs(p[0]) > abs(p[2]))
			return vec_type(p[0] > 0 ? 1 : -1, 0, 0);
		else if (abs(p[1]) > abs(p[2]))
			return vec_type(0, p[1] > 0 ? 1 : -1, 0);
		else
			return vec_type(0, 0, p[2] > 0 ? 1 : -1);
	}

	void create_gui()
	{
		implicit_primitive<T>::create_gui();
	}
};

scene_factory_registration<box<double> > sfr_box("box;B");
