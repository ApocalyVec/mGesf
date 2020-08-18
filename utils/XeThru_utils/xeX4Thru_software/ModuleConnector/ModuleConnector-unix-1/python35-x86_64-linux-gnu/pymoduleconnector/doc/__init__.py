def show():
    """ Opens pymoduleconnector documentation with system default webbrowser.
    """
    from os.path import dirname, realpath, join
    from webbrowser import open
    
    index = join(dirname(realpath(__file__)), 'output', 'html','index.xhtml')
    open(index)
