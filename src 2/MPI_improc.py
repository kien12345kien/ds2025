from mpi4py import MPI
from PIL import Image
import numpy as np
import os

'''
Installation:
    [Linux and Windows]
    conda install -c conda-forge mpi4py impi_rt

Run command:
    mpirun -np 4 python mpi_improc.py

For Windows user, please run:
    mpiexec -np 4 python mpi_improc.py

Where -np 4 specifies 4 processes. Adjust this number based on your system.

Explanation:
    MPI_COMM_WORLD: Represents all the processes running the program.
    Rank: Identifies each process. Rank 0 is typically the coordinator.
    Scatter: Distributes data from one process (root) to all processes.
    Gather: Collects data from all processes back to one process (root).
'''

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Input and output image paths
input_image_path = "input_image.png"  # Replace with your input image filename
output_image_path = "output_image.png"

if rank == 0:
    # Root process: Read the image and divide it into chunks
    image = Image.open(input_image_path)
    image_array = np.array(image)
    
    # Split the image array into chunks along the height
    chunks = np.array_split(image_array, size, axis=0)
else:
    # Non-root processes: Initialize empty chunk
    chunks = None

# Scatter chunks to all processes
chunk = comm.scatter(chunks, root=0)

# Each process converts its chunk to grayscale
grayscale_chunk = np.mean(chunk, axis=2).astype(np.uint8)

# grayscale_chunk = chunk.convert("L")

# Gather grayscale chunks back to the root process
grayscale_chunks = comm.gather(grayscale_chunk, root=0)

if rank == 0:
    # Root process: Reconstruct the full grayscale image
    grayscale_image = np.vstack(grayscale_chunks)
    output_image = Image.fromarray(grayscale_image)
    output_image.save(output_image_path)
    print(f"Grayscale image saved as {output_image_path}")
