import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="pythonfuzz",
    version="1.0.10",
    author="GitLab B.V.",
    description="Coverage-guided fuzz testing for Python",
    long_description=long_description,
    long_description_content_type="text/markdown",
    install_requires=[
        # WARNING: Keep these values in line with those in requirements.txt
        "psutil==5.6.3",
        "functools32==3.2.3.post2; python_version < '3'",
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Topic :: Software Development :: Testing"
    ],
    python_requires='>=3.5.3',
    packages=setuptools.find_packages('.', exclude=("examples",))
)
