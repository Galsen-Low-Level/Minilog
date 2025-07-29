# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Minilog'
copyright = '2025, Galsen Low Level'
author = 'Umar Ba <jUmarB@protonmail.com>'
release = '1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration


import os
import sys
sys.path.insert(0, os.path.abspath('.'))

extensions = [
    'breathe',
    'sphinx.ext.todo', 
    'sphinx.ext.autodoc',
]

breathe_projects = {
    "Minilog": "./_dox/xml"
}
breathe_default_project = "Minilog" 

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_logo = "../assets/logo/mlog-logo-2.png"
html_theme = 'sphinx_nefertiti'
html_static_path = ['_static']
