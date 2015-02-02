package jamtaba.command;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Elieser
 */
public interface Command {

    void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException;

    boolean requiredDataIsAvailable(HttpServletRequest req);
    

}
