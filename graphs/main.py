from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import PolyCollection
import matplotlib.pyplot as plt
from matplotlib import colors as mcolors
import numpy as np
from matplotlib.ticker import FormatStrFormatter, MultipleLocator


def cc(arg, alpha=0.6):
	return mcolors.to_rgba(arg, alpha=alpha)

def items():

	# First column is threads
	# Second column is size of problem
	# Third column is execution time

	with open('../results/openmp_schedule.txt', 'r') as f:
		content = f.readlines()

	# Every data I need, but as strings
	content = [x.strip().split() for x in content]
	results = []

	for item in content:
		curr_result = []
		curr_result.append(int(item[0]))
		curr_result.append(int(item[1]))
		curr_result.append(float(item[2]))
		results.append(curr_result)

	return results


def main():
	# Creation of the 3D model
	fig = plt.figure()
	ax = fig.gca(projection='3d')

	# The range of size in which the graphs will be displayed
	xs = np.arange(10, 16)
	#xs = np.linspace(10., 16., 5)
	verts = []

	# The proccessor that the graph will be placed
	zs = [1, 2, 3, 4]
	content = items()

	# Repeat for every different graph
	for z in zs:

		ys = []
		ys.append(0)
		correct_size = 11
		for item in content:
			print(item)
			# If this is the correct number of threads
			if item[0] == z:

				# If this is the correct size
				if item[1] == correct_size:

					print("mpainei")
					# Add the time to the index of this size
					ys.append(item[2])

					# Go to the next size
					correct_size += 1

		# indexes 0 and -1 are equal to 0 because otherwise the graphs would float
		ys[-1] = 0

		# Add to list the values and the place in their graph (the size)
		verts.append(list(zip(xs, ys)))


	poly = PolyCollection(verts, facecolors=[cc('red'), cc('g'), cc('b'), cc('purple')])

	#adiafaneia
	#poly.set_alpha(0.7)
	ax.add_collection3d(poly, zs=zs, zdir='y')

	#Label axes and give the scale of them
	ax.set_xlabel('Size')
	ax.set_xlim	(10, 15)
	ax.set_ylabel('Proccessors')
	ax.set_ylim(1, 5)
	ax.set_zlabel('Time')
	ax.set_zlim(0, 4)

	#Display cores as integers
	majorLocator   = MultipleLocator(1)
	ax.yaxis.set_major_locator(majorLocator)
	yFormatter = FormatStrFormatter('%d')
	ax.yaxis.set_major_formatter(yFormatter)

	#Display size as integers
	majorLocator2 = MultipleLocator(1)
	ax.xaxis.set_major_locator(majorLocator2)
	xFormatter = FormatStrFormatter('%d')
	ax.yaxis.set_major_formatter(xFormatter)


	# display the model
	plt.show()


main()
