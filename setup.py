from setuptools import setup, find_packages, Extension

# Define the extension module with the correct path to the C file
moving_avg_module = Extension(
    'moving_avg',  # Name of the module
    sources=['optimisation/moving_avg_module.c'],  # Source file with correct path
    include_dirs=['C:/Users/hrish/AppData/Local/Programs/Python/Python312/include'],  # Include directory for Python headers
    library_dirs=['C:/Users/hrish/AppData/Local/Programs/Python/Python312/libs'],  # Library directory for Python libraries
    libraries=['python312']  # Python library to link against
)

setup(
    name='trading_bot_pipeline',
    version='0.1',
    packages=find_packages(),
    install_requires=[
        # List your project's dependencies here
    ],
    entry_points={
        'console_scripts': [
            # Define command-line scripts here
        ],
    },
    ext_modules=[moving_avg_module]  # Add the extension module here
)
