# conecylinj-model

- ConeCylinderInjection: spray kinematic submodel that extends the 2D disc injection into 3D cylinderical "volumetric" injection.

- Derived from InjectionModel in stock, and inheritance level of "ConeCylinderInjection" model is the same as "ConeInjectionModel".

- coneCylinderInjection model allows only method "cylinder", to avoid code duplication for "point" or "disc" which are already implemented in coneInjection model.

- The runtime warning that we previously encountered for undefined symbols was actually due to some missing library ("liblagrangianSpray" in particular) that needs to be linked. Therefore, nothing to do with duplicate implementation of "point" and "disc" related member functions.



# References

