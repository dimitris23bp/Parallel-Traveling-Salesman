from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import PolyCollection
import matplotlib.pyplot as plt
from matplotlib import colors as mcolors
import numpy as np

# Creation of the 3D model
fig = plt.figure()
ax = fig.gca(projection='3d')


def cc(arg, alpha=0.6):
	return mcolors.to_rgba(arg, alpha=alpha)

# The range of size in which the graphs will be displayed 
xs = np.arange(0, 11)

verts = []

# The proccessor that the graph will be placed
zs = [1, 2, 3, 4, 1]

flag = True
# Repeat for every different graph
for z in zs:
	if flag == True:
		ys = [0, 3.1, 1.2, 1.6, 1.5, 2.3, 1.2, .7, .2, .9, .4]
		flag = False
	else:
		ys = [0, 3.1, .2, .6, .5, .3, 3.2, .7, .2, .9, .4]

	# indexes 0 and -1 are equal to 0 because otherwise the graphs would float
	ys[0], ys[-1] = 0, 0

	# Add to list the values and the place in their graph (the size)
	verts.append(list(zip(xs, ys)))


poly = PolyCollection(verts, facecolors=[cc('white'), cc('g'), cc('b'), cc('purple'), cc('black', 1)])

#adiafaneia
#poly.set_alpha(0.7)
ax.add_collection3d(poly, zs=zs, zdir='y')

#Label axes and give the scale of them
ax.set_xlabel('Size')
ax.set_xlim3d(0, 10)
ax.set_ylabel('Proccessors')
ax.set_ylim3d(1, 5)
ax.set_zlabel('Time')
ax.set_zlim3d(0, 4)

# display the model
plt.show()