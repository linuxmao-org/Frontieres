#include "visual/Circular.h"


Circular::Circular(double s,double xOr, double yOr,double r, double ang, double stch, double prgs)
    : Trajectory(s,xOr,yOr)
{
    pt2d  orig=getOrigin();
    this->centerX = orig.x;
    this->centerY = orig.y;
    setRadius(r);
    setDistanceToCenter(0);
    setAngle(ang);
    setStretch(stch);
    setProgress(prgs);

}

pt2d Circular::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph = this->getPhase();
    const double PI  =3.141592653589793238463;
    ph+=sp*dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph-int(ph));

    //see Euler spiral for a smooth passage from a spiral to a circle
    if (getDistanceToCenter() <= getRadius())
    setDistanceToCenter(getDistanceToCenter() + getRadius() / getProgress());

    //the modulus and the angle, a polar complex number
    double rho = (getDistanceToCenter() < getRadius()) ? getDistanceToCenter() : getRadius();
    double theta = ph * 2 * PI;

    // Valeurs (Exemple) ////
    const double monAngle = - getAngle() * (PI / 180.); // rotation 45°
    const double stretchX = getStretch(); // étirement horizontal
    const double stretchY = 1; // étirement vertical
    // Fin exemple       ////

    // convert to cartesian (real=X, imag=Y)
    std::complex<double> cartesian = std::polar(rho, theta);

    // stretch first
    cartesian = std::complex<double>(cartesian.real() * stretchX, cartesian.imag() * stretchY);

    // back to polar
    rho = std::abs(cartesian);
    theta = std::arg(cartesian);

    // then rotate, using polar coordinates
    theta += monAngle;

    // back to cartesian
    cartesian = std::polar(rho, theta);

    // add origin
    pt2d orig = getOrigin();
    return pt2d(orig.x + (float)cartesian.real(), orig.y + (float)cartesian.imag());
}

double Circular::getCenterX()
{
    return centerX;
}

double Circular::getCenterY()
{
    return centerY;
}


int Circular::getType()
{
    return CIRCULAR;
}

Circular::~Circular()
{
}
