package main_pack;
import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableModel;

public class JTableUtilities
{
    //public static void setCellsAlignment(JTable table, int alignment)
    public static void setCellsAlignment(JTable table)
    {
        DefaultTableCellRenderer rightRenderer = new DefaultTableCellRenderer();
        rightRenderer.setHorizontalAlignment(SwingConstants.CENTER);
        
        DefaultTableCellRenderer leftRenderer = new DefaultTableCellRenderer();
        leftRenderer.setHorizontalAlignment(SwingConstants.LEFT);
        
        TableModel tableModel = table.getModel();
        
        table.getColumnModel().getColumn(0).setCellRenderer(leftRenderer);
        
        for (int columnIndex = 1; columnIndex < tableModel.getColumnCount(); columnIndex++)
        {
            table.getColumnModel().getColumn(columnIndex).setCellRenderer(rightRenderer);
        }
    }
}
