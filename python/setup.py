"""setup module for zabbix_module"""

from setuptools import setup, find_packages
import zabbix_module

setup(
    name = 'zabbix_module',

    version = zabbix_module.__version__,

    description = 'A sample Python project',

    author = 'Ryan Armstrong',

    author_email = 'ryan@cavaliercoder.com',

    url='https://github.com/cavaliercoder/libzbxpython',

    license = 'MIT',

    # See https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
    ],

    keywords = 'zabbix',

    packages = [ 'zabbix_module' ],
)
