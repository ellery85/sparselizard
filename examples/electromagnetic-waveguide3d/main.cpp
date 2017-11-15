#include "mesh.h"
#include "field.h"
#include "expression.h"
#include "formulation.h"
#include "vec.h"
#include "petsc.h"
#include "wallclock.h"
#include "mat.h"


using namespace mathop;

void sparselizard(void)
{	
    // The domain regions as defined in 'waveguide3D.geo':
    int left = 1, skin = 2, wholedomain = 3;

    mesh mymesh("waveguide3D.msh");

    // Edge shape functions 'hcurl' for the electric field E.
    // Fields x, y and z are the x, y and z coordinate fields.
    field E("hcurl"), x("x"), y("y"), z("z");

    // Use interpolation order 2 on the whole domain:
    E.setorder(wholedomain, 2);
    
    // The cutoff frequency for 0.2x0.2 cross section is freq = 1.06 GHz in theory. 
    double freq = 1.2e9, c = 3e8, pi = 3.14159, k = 2*pi*freq/c;
    
    // The waveguide a perfect conductor. We thus force all
    // tangential components of E to 0 on the waveguide skin.
    E.setconstraint(skin);
    // We force an electric field in the z direction on region 'left'
    // that is 0 on the exterior of 'left' and 1 in the center.
    E.setconstraint(left, cos(y/0.2*pi)* cos(z/0.2*pi)* array3x1(0,0,1));

    formulation maxwell;
    
    // This is the weak formulation for electromagnetic waves:
    maxwell += integral(wholedomain, -curl(dof(E))*curl(tf(E)) + k*k*dof(E)*tf(E));
    
    // The operations below take about a minutes on my laptop... be patient!
    maxwell.generate();
    vec solE = solve(maxwell.A(), maxwell.b());
    
    E.getdata(wholedomain, solE);    
    // Save the electric field E and magnetic field H with an order 2 interpolation:
    curl(E).write(wholedomain, "H.pos", 2);
    E.write(wholedomain, "E.pos", 2);
}

int main(void)
{	
    PetscInitialize(0,{},0,0);

    sparselizard();

    PetscFinalize();

    return 0;
}









