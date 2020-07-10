#ifndef _es_ch_plot_geometry_
#define _es_ch_plot_geometry_

namespace EsChPlotGeometry
{
	namespace _2D 
	{
		namespace Cartesian
		{
      /// @brief  Return an absolute distance between two 2D points
      ///
      /// @param	x0          X-coordinate of the first 2D point.
      /// @param  y0	        Y-coordinate of the first 2D point.
      /// @param  x1	        X-coordinate of the second 2D point.
      /// @param	y1          Y-coordinate of the second 2D point.
      /// 
      /// @returns            an absolute distance between two points on plane
      ///
			ESCHART_FUNC( double, distanceGet(double x0, double y0, double x1, double y1) );

      /// @brief  Check if two 2D points are in vicinity of each other.
      ///
      /// @param	x0          X-coordinate of the first 2D point.
      /// @param  y0	        Y-coordinate of the first 2D point.
      /// @param  x1	        X-coordinate of the second 2D point.
      /// @param	y1          Y-coordinate of the second 2D point.
      /// @param  toleranceX  Vicinity check tolerance in direction of X-axis
      /// @param  toleranceY  Vicinity check tolerance in direction of Y-axis
      /// 
      /// @returns            true, if points are in vicinity, false otherwise
      ///
			ESCHART_FUNC( bool, pointsInVicinity(double x0, double y0, double x1, double y1, double toleranceX, double toleranceY) );
		}
	}
}

#endif // _es_ch_plot_geometry_